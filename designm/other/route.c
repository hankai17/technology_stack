#include <stddef.h>
#include <unistd.h>

static char iproute_head[IP_ROUTE_CMD_MAX][IP_ROUTE_HEAD_SIZE] = {
	"ip route", "ip -6 route"
};

static char iproute_option[IP_ROUTE_OPTION_MAX][IP_ROUTE_OPTION_SIZE] = {
	"add", "del", "show"
};

static char route_maskipv6_arr[ROUTE_MASKIPV6_MAX][LONG_LENGTH] = {
	"fe80", "fe00"
};

void minitos_debug_mroute(struct minitos_route *mroute){
	char show_buf[CMD_LENGTH] = {'\0'};
	char dst[MIDDLE_LENGTH] = {0};
	char gw[MIDDLE_LENGTH] = {0};

	int ret = 0;
	size_t len = 0;
	//dst
		
	ret = minitos_format_saddr(&mroute->dst, dst, MIDDLE_LENGTH);
	if(ret < 0)
		dst[0] = '\0';
	len += snprintf(show_buf + len, sizeof(show_buf) - len, "dst = %s/%u\t", dst, mroute->dst.subnet);
	minitos_format_saddr(&mroute->gw, gw, MIDDLE_LENGTH);
	len += snprintf(show_buf + len, sizeof(show_buf) - len, "gw = %s\t", gw);
	len += snprintf(show_buf + len, sizeof(show_buf) - len, "metric = %u\t", mroute->metric);
	len += snprintf(show_buf + len, sizeof(show_buf) - len, "dev= %s\t", mroute->dev);

	ROUTE_DEBUG("the mroute is %s", show_buf);
}

static int del_one_route(char *dst, char *gw, char *netmask, char *flags, char *metric, char *ref, char *use, char *iface)
{
	int ret = 0;
	int ip_flag = -1;
	char cmd[CMD_LENGTH] = {0};
	char *retbuf = NULL;

	if (!strlen(netmask)){
		ip_flag = IP_TYPE_V6;
	}else{
		ip_flag = IP_TYPE_V4;
	}
	if (ip_flag == IP_TYPE_V4){
		if (strstr(flags, "H")){
			sprintf(cmd, "route del -host %s", dst);
		}else{
			sprintf(cmd, "route del -net %s netmask %s", dst, netmask);
		}
		if (strcmp(gw, "*")){
			sprintf(cmd, "%s gw %s", cmd, gw);
		}
		if (strstr(flags, "G")){
			sprintf(cmd, "%s dev %s", cmd, iface);
		}
	}else if (ip_flag == IP_TYPE_V6){
		sprintf(cmd, "route -A inet6 del %s", dst);		
		if (strcmp(gw, "::")){
			sprintf(cmd, "%s gw %s", cmd, gw);
		}
		if (strstr(flags, "G")){
			sprintf(cmd, "%s dev %s", cmd, iface);
		}
		if (strlen(metric)){
			sprintf(cmd, "%s metric %s", cmd, metric);
		}
	}
	
	ret = process_cmd(cmd, &retbuf, LONG_LENGTH);
	if (ret > 0){
		ret = -CMD_ERROR;
		goto out;
	}
out:
	if (retbuf){
		free(retbuf);
		retbuf = NULL;
	}
	return ret;
}

int minitos_del_route2kerel(struct minitos_route *mroute){
	int ret = 0;
	char buf[CMD_LENGTH];
	if((ret = minitos_format_cmd(mroute, IP_ROUTE_DEL, buf, CMD_LENGTH))< 0){
		ROUTE_DEBUG("format cmd error");
		goto end;
	}
	
	if((ret = minitos_iproute_execmd(buf, sizeof(buf))) < 0){
		ROUTE_DEBUG("route add exec cmd error");
	}

end:
	return ret;
}

int minitos_add_route2kernel(struct minitos_route *mroute){
	int ret = 0;
	char buf[CMD_LENGTH];

	if((ret = minitos_format_cmd(mroute, IP_ROUTE_ADD, buf, CMD_LENGTH))< 0){
		ROUTE_DEBUG("format cmd error");
		goto end;
	}
	
	if((ret = minitos_iproute_execmd(buf, sizeof(buf))) < 0){
		ROUTE_DEBUG("route add exec cmd error");
	}

end:
	return ret;
}

int minitos_format_filter_mroute_save(struct minitos_routeshow * routeshow,
		struct minitos_route *pmroute){
	return  minitos_format_filter_mroute_none(routeshow, pmroute);
}

int minitos_format_filter_mroute_snmp(struct minitos_routeshow * routeshow,
		struct minitos_route *pmroute){
	return  minitos_format_filter_mroute_none(routeshow, pmroute);
}

/////////////////////////////////////////抽象层 用于回调///////////////////////////////////////////////////////////
int  mtos_getroute2local(struct minitos_routeshow *routeshow, struct minitos_route *pmroute, get_from_l2do func) {
	unsigned int vsid = 0;
	int ret = 0;
	size_t len = 0;
	int num = 0;
	int i = 0;
	
	struct minitos_slist_head *mslist_head = &routeshow->slist_head[MTOS_SLIST_FOR_USER];
	struct minitos_route *cmroute = NULL;
	struct tos_obj_head *obj = NULL;
	char *databuf = NULL;

	ret = minitos_showroute_by_vs2configd(&databuf, vsid, &len);
	if(ret < 0){
		ROUTE_DEBUG("get mroute from configd error");
		goto end;
	}
	num = minitos_showroute_num2configd(vsid);
	obj = (struct tos_obj_head *)databuf;
	for(i = 0; i < num; i++) {
		cmroute = (struct minitos_route *)((char *)obj + sizeof(struct tos_obj_head));
		if(routeshow){
			ret = routeshow->func.ftmroute(routeshow, cmroute); // 遍历每个obj 调抽象结构体的回调函数
		}
		ret = func(mslist_head, cmroute, pmroute, obj->ID); // 依赖反转
next:
		obj = (struct tos_obj_head *)((char *)obj + sizeof(struct tos_obj_head) + obj->data_len);
	}
end:
	if(databuf)
		free(databuf);
	return ret;
}

///////////////////////////////////////obj clean callback////////////////////////////////////////////////////
int minitos_format_filter_mroute_clean(struct minitos_routeshow * routeshow,
		struct minitos_route *pmroute) {
	int flag = routeshow->filter_route.flag;
	if(!(flag & MINITOS_FLAG_SHOW_ALL)){
		if(flag & MINITOS_FLAG_SHOW_IPV4){
			if(pmroute->dst.flag & MINITOS_IPV4){
				goto end;
			}else{
				goto error;	
			}
		}else if(flag & MINITOS_FLAG_SHOW_IPV6){
			if(pmroute->dst.flag & MINITOS_IPV6){
				goto end;
			}else{
				goto error;	
			}
		}
	}
end:
	return 0;
error:
	return -1;

}

int minitos_format_filter_mroute_netlink(struct minitos_routeshow * routeshow,
		struct minitos_route *pmroute){
	int filter_flag = routeshow->filter_route.flag;
	if(filter_flag & MINITOS_FLAG_NONE_DEV ||pmroute->flag & MINITOS_FLAG_NONE_DEV){
		goto end;	
	}else{
		if(!strncmp(routeshow->filter_route.dev, pmroute->dev, sizeof(routeshow->filter_route.dev))){
			goto end;
		}else{
			goto error;	
		}
	}
end:
	return 0;
error:
	return -1;
}

int minitos_format_filter_mroute(struct minitos_routeshow* routeshow, 
		struct minitos_route *pmroute){
	struct minitos_route *filter_mroute = &routeshow->filter_route;
	unsigned int flag = filter_mroute->flag;
	unsigned int same_count = 0;
	unsigned int all_count = 0;
	if(!(flag & MINITOS_FLAG_SHOW_ALL)){
		all_count++;
		if(flag & MINITOS_FLAG_SHOW_IPV4){
			if(pmroute->dst.flag & MINITOS_IPV4){
				same_count++;
			}
		}else if(flag & MINITOS_FLAG_SHOW_IPV6){
			if(pmroute->dst.flag & MINITOS_IPV6){
				same_count++;	
			}
		}
	}

	if(!(flag & MINITOS_FLAG_NONE_DST)){
		all_count++;
		if(!memcmp(&filter_mroute->dst, &pmroute->dst, sizeof(struct minitos_addr))){
			same_count++;
		}else{
			ROUTE_DEBUG("the filter_mroute is not same");
		}
	}
	
	if(!(flag & MINITOS_FLAG_NONE_GW)){
		all_count++;
		if(!memcmp(&filter_mroute->gw, &pmroute->gw, sizeof(struct minitos_addr))){
			same_count++;	
		}
	}

	if(!(flag & MINITOS_FLAG_NONE_DEV)){
		all_count++;
		if(pmroute->flag & MINITOS_FLAG_NONE_DEV || !strncmp(filter_mroute->dev, pmroute->dev, sizeof(filter_mroute->dev))){
			same_count++;	
		}
	}

	if(!(flag & MINITOS_FLAG_NONE_METRIC)){
		all_count++;
		if(filter_mroute->metric == pmroute->metric){
			same_count++;	
		}
	}
	ROUTE_DEBUG("the same count is %u, all count is %u", same_count, all_count);

	if(same_count == all_count)
		return 0;
	else
		return -1;
}

///////////////////////////////暴露给cli/ui////////////////////////////////////////////////////////////////
// 设routeshow为 回调对象 里面有flag func policy
// 这种模式/套路就是:   初始化回调对象 + 依赖反转, 我那种没有回调对象 只是一个简单的依赖反转

// add的时候跟回调对象无关 只用了简单的依赖反转
int route_add(const manager_info_t * minfo, int argc, char **argv, int *buflen, char **buf)
{	
	int ret = 0;
	unsigned int vsid = minfo->vsid;
	struct minitos_route mroute;
	struct minitos_addr *dst_maddr = &mroute.dst;
	struct minitos_addr *gw_maddr = &mroute.gw;
	static int sleep_flag = 0;
	int num = 0;

	memset(&mroute, 0, sizeof(mroute));
	print_input(argc, argv);
	num = minitos_showroute_num2configd(vsid);
	if(num > ROUTE_CONFIG_NUM_MAX){
		ret = -RT_EFIBFULL;
		goto end;
	}
	if((ret = minitos_format_mroute(&mroute, argc, argv)) < 0){
		ROUTE_DEBUG("format mroute error");
		goto end;
	}
	ret = mtos_getroute2local(NULL, &mroute,  minitos_find_route_inconfigd); // 依赖反转
	ret = minitos_add_route2configd(&mroute, vsid); // 加入全局对象中
	ret = minitos_add_route2kernel(&mroute);        // 加入全局内核 就是执行命令
end:
	return ret;
}

// del某一个路由的时候 跟回调对象有关 先根据参数初始化回调对象 然后依赖反转 匹配该路由
int route_del(const manager_info_t * minfo, int argc, char **argv, int *buflen, char **buf)
{	
	int ret = 0;
	int id = 0;
	struct minitos_route mroute;
	unsigned int vsid = minfo->vsid;
	struct minitos_routeshow   routeshow;
	struct minitos_slist_head *mslist_head_user = &routeshow.slist_head[MTOS_SLIST_FOR_USER];
	struct minitos_slist_head *mslist_head_kernel = &routeshow.slist_head[MTOS_SLIST_FOR_KERNEL];
	char dst[MIDDLE_LENGTH] = {0};
	char *gw = NULL;

	ret = route_del_trans_dstgw(minfo, argv, dst, sizeof(dst), &gw);
	mtos_init_delete_routeshow(&routeshow);                                        // 初始化 回调对象 
	ret = minitos_format_mroute_filter(&routeshow.filter_route, argv[RT_PARAM_DEL_AF],	\
			dst, gw, argv[RT_PARAM_DEL_DEV], argv[RT_PARAM_DEL_METRIC]);

	ret = mtos_route_init_showlist(&routeshow);	// 把原始对象中的对象 user_list中 内核也加到kernel_list中 
	minitos_debug_showlist(&routeshow);
	ret = mtos_get_and_format_showbuf(&routeshow, (int*)&delete_buflen, &delete_buf);
	if(ret < 0)
		goto end;
	if(delete_buf && delete_buflen)
		mini_system(delete_buf);
	else
		ret = -RT_ENOENTRY;
end:
	minitos_mslist_free(mslist_head_user);
	minitos_mslist_free(mslist_head_kernel);
	return ret;
}

int route_clean(const manager_info_t * minfo, int argc, char **argv, int *buflen, char **buf)
{
	int ret = 0;
	struct minitos_routeshow routeshow;
	mtos_init_clean_routeshow(&routeshow);                                  // 初始化 回调对象 的flag 以及回调函数为 minitos_format_filter_mroute_clean
	minitos_format_mroute_filter(&routeshow.filter_route, argv[0])) < 0);   // 根据传参 初始化 回调对象 的 policy
	mtos_getroute2local(&routeshow, NULL, minitos_clean_route_inconfigd); 	// 依赖反转  ---> 1)删除内核的 2)删除每个对象中
end:
	return ret;
}

int route_show(const manager_info_t * minfo, int argc, char **argv, int *buflen, char **buf){
	unsigned int vsid = 0;
	int ret = 0;
	struct minitos_routeshow   routeshow;
	struct minitos_slist_head *mslist_head_user = &routeshow.slist_head[MTOS_SLIST_FOR_USER];
	struct minitos_slist_head *mslist_head_kernel = &routeshow.slist_head[MTOS_SLIST_FOR_KERNEL];
	mtos_init_cli_routeshow(&routeshow);                                                            // 初始化 回调对象 中的 2对三个链表(user/kernel) show_list/ipv4/ipv6_list 诸多回调
	ret = minitos_format_mroute_filter(&routeshow.filter_route, argv[RT_PARAM_SHOW_TYPE],           // 根据传参初始化 回调对象 中的 policy
	ret = mtos_route_init_showlist(&routeshow);                                                     // 把原始对象中的对象 user_list中 内核也加到kernel_list中 
	ret = mtos_get_and_format_showbuf(&routeshow, buflen, buf);
end:
	minitos_mslist_free(mslist_head_user);
	minitos_mslist_free(mslist_head_kernel);
	return ret;
	goto end;
}

void _minitos_debug_showlist(struct minitos_showlist *pos, int index){
	char show_buf[CMD_LENGTH] = {0};
	char dst[MIDDLE_LENGTH] = {0};
	char gw[MIDDLE_LENGTH] = {0};
	int ret = 0;
	
	size_t len = 0;

	len += snprintf(show_buf + len, sizeof(show_buf) - len, "index %d:id = %u\t", index, pos->id);
	ret = minitos_format_cmd_dst(&pos->mroute.dst, dst, sizeof(dst));
	if(ret < 0)
		dst[0] = '\0';
	len += snprintf(show_buf + len, sizeof(show_buf) - len, "dst = %s\t", dst);
	
	if(pos->mroute.flag & MINITOS_FLAG_NONE_GW){
		strncpy(gw, "none", sizeof(gw));
	}else{
		ret = minitos_format_saddr(&(pos->mroute.gw), gw, sizeof(gw));
		if(ret < 0)
			gw[0] = '\0';
	}

	len += snprintf(show_buf + len, sizeof(show_buf) - len, "gw = %s\t", gw);

	len += snprintf(show_buf + len, sizeof(show_buf) - len, "dev = %s", pos->mroute.dev);

	
	len += snprintf(show_buf + len, sizeof(show_buf) - len, "dev = %s", pos->mroute.dev);
	ROUTE_DEBUG("%s", show_buf);
	return;
}

void minitos_debug_showlist_by_type(struct list_head *list_head, unsigned int type){
	int i = 0;
	struct minitos_showlist *pos = NULL;
	struct minitos_showlist *n = NULL;
	if(route_list_empty(list_head)){
		ROUTE_DEBUG("the list is empty");
		goto end;
	}
	switch(type){
		case MTOS_LIST_HEAD_SHOW:
			list_for_each_entry_safe(pos, n, list_head, show_list){
				_minitos_debug_showlist(pos, i);
				i++;
			}
			break;
		case MTOS_LIST_HEAD_FA:
			list_for_each_entry_safe(pos, n, list_head, family_list){
				_minitos_debug_showlist(pos, i);
				i++;
			}
			break;
		default:
			break;
	}
end:
	return;
}
void minitos_debug_showlist(struct minitos_routeshow *routeshow){
	int ret = 0;
	struct list_head *list_head = NULL;
	struct minitos_slist_head *head_for_user = &routeshow->slist_head[MTOS_SLIST_FOR_USER];
	struct minitos_slist_head *head_for_kernel = &routeshow->slist_head[MTOS_SLIST_FOR_KERNEL];

	ROUTE_DEBUG("\n\n========debug showlist begin=========");
	ROUTE_DEBUG("\n========debug user begin=========");
	if(head_for_user->is_init != 0){
		ROUTE_DEBUG("\n========debug all list =========");
		minitos_debug_showlist_by_type(&head_for_user->show_list, MTOS_LIST_HEAD_SHOW);
		ROUTE_DEBUG("\n========debug ipv4 list =========");
		minitos_debug_showlist_by_type(&head_for_user->ipv4_list, MTOS_LIST_HEAD_FA);
		ROUTE_DEBUG("\n========debug ipv6 list =========");
		minitos_debug_showlist_by_type(&head_for_user->ipv6_list, MTOS_LIST_HEAD_FA);
	}
	ROUTE_DEBUG("\n\n========debug user end=========");
	
	ROUTE_DEBUG("\n========debug kernel begin=========");
	if(head_for_user->is_init != 0){
		ROUTE_DEBUG("\n========debug all list =========");
		minitos_debug_showlist_by_type(&head_for_kernel->show_list, MTOS_LIST_HEAD_SHOW);
		ROUTE_DEBUG("\n========debug ipv4 list =========");
		minitos_debug_showlist_by_type(&head_for_kernel->ipv4_list, MTOS_LIST_HEAD_FA);
		ROUTE_DEBUG("\n========debug ipv6 list =========");
		minitos_debug_showlist_by_type(&head_for_kernel->ipv6_list, MTOS_LIST_HEAD_FA);
	}
	ROUTE_DEBUG("\n\n========debug kernel end=========");

	ROUTE_DEBUG("\n\n========debug showlist end========= \n");
	return;
}

void minitos_list_free(struct list_head *list_head, unsigned int type){
	struct minitos_showlist *pos = NULL;
	struct minitos_showlist *n = NULL;
	if(route_list_empty(list_head)){
		ROUTE_DEBUG("the list is empty, don't need free");
		goto end;
	}

	switch(type){
		case MTOS_LIST_HEAD_SHOW:
			list_for_each_entry_safe(pos, n, list_head, show_list){
				list_del(&pos->show_list);
				list_del(&pos->family_list);
				free(pos);
			}
		break;
		default :
			break;
	}
end:
	return ;
}

void minitos_mslist_free(struct minitos_slist_head *mslist_head){
	minitos_list_free(&mslist_head->show_list, MTOS_LIST_HEAD_SHOW);
}

static void minitos_add2list(struct list_head *add ,struct list_head *head){
	list_add(add, head);
	return;
}

int minitos_clean_route_inconfigd(struct minitos_slist_head *none_l, 
	struct minitos_route *pmroute,	struct minitos_route *none_r, unsigned int id) { // 原始对象 回调对象
	minitos_del_route2kerel(pmroute);
	minitos_del_route2configd(id, 0);
	return 0;
}

int minitos_find_route_inconfigd(struct minitos_slist_head *none_l, 
        struct minitos_route *pmroute, struct minitos_route *cmroute, unsigned int none_id) {
	int ret = 0;
	if(!memcmp(&cmroute->dst, &pmroute->dst, sizeof(struct minitos_addr))){
		if(pmroute->metric == cmroute->metric){
			ret = -RT_EEXIST;
			goto end;
		}
	}
end:
		return ret;
}

int minitos_addmroute2showlist(struct minitos_slist_head *mslist_head, 
        struct minitos_route *mroute, struct minitos_route *none_r, unsigned int id){ // OBJ中的每个对象分配一个showlist 挂到 回调对象 的user_list中
	int ret = 0;
	struct minitos_showlist *tmp_show_list = NULL;
	//add to show list
	tmp_show_list = (struct minitos_showlist *)malloc(sizeof(struct minitos_showlist));
	memset(tmp_show_list, 0, sizeof(struct minitos_showlist));
	memcpy(&tmp_show_list->mroute, mroute, sizeof(struct minitos_route));
	tmp_show_list->id = id;
	//if id = 0, the route must be get from kernel this route is up status
	if(id == 0)
		tmp_show_list->flag |= MINITOS_FLAG_ROUTE_UP;
	//add to show list
	minitos_add2list(&tmp_show_list->show_list, &mslist_head->show_list);
	mslist_head->showlist_num++;
	//add to index list
	if(mroute->dst.flag & MINITOS_IPV4){
		minitos_add2list(&tmp_show_list->family_list, &mslist_head->ipv4_list);
		mslist_head->ipv4list_num++;
	}else if(mroute->dst.flag & MINITOS_IPV6){
		minitos_add2list(&tmp_show_list->family_list, &mslist_head->ipv6_list);
		mslist_head->ipv6list_num++;
	}
end:
	return ret;
}

void minitos_find2list(struct minitos_route *pmroute, \
		struct list_head *list_head){
	struct minitos_showlist *pos = NULL;
	struct minitos_showlist *n = NULL;

	list_for_each_entry_safe(pos, n, list_head, family_list){
		if(!memcmp(&pos->mroute.dst, &pmroute->dst, sizeof(struct minitos_addr))){
			if(pmroute->metric == pos->mroute.metric){
				pos->flag |= MINITOS_FLAG_ROUTE_UP;
				if(pos->mroute.flag & MINITOS_FLAG_NONE_DEV){
					strncpy(pos->mroute.dev, pmroute->dev, sizeof(pos->mroute.dev));
				}
				break;
			}
		}
	}
end:
	return;
}

int mtos_getroute_flagipv_2kernel(unsigned int flag, struct list_head *list_head){ // 内核每一行初始化成一个route 从user_list中查找 ???
	int ret = 0;
	FILE *fp = NULL;
	char cmd[CMD_LENGTH] = {0};
	char read_buf[CMD_LENGTH] = {0};
	char dev[LONG_LENGTH] = {0};
	struct minitos_route mroute;
	
	//format show_cmd
	snprintf(cmd, sizeof(cmd), "%s %s |grep -v '%s'", get_ip_route_cmd_head(flag),
			iproute_option[IP_ROUTE_SHOW], IP_ROUTE_PROTO_KERNEL);
	fp = popen(cmd, "r");
	while(fgets(read_buf, sizeof(read_buf), fp)){
		memset(&mroute, 0, sizeof(mroute));
		// mask this route is 'proto kernel' attribute
		ret = mtos_format_mroute_by_kernelshow(&mroute, read_buf, flag);
		if(ret < 0){
			ret = 0;
			continue;
		}
		minitos_find2list(&mroute, list_head);
	}
end:
	if(fp)
		pclose(fp);
	return ret;
}

int mtos_getroute_flag2kernel(struct minitos_slist_head *mslist_head){
	int ret = 0;
	ret = mtos_getroute_flagipv_2kernel(MINITOS_IPV4, &mslist_head->ipv4_list);
	ret = mtos_getroute_flagipv_2kernel(MINITOS_IPV6, &mslist_head->ipv6_list);
end:
	return ret;
}

int mtos_getroute2kernel_ipv(struct minitos_routeshow *routeshow, unsigned int flag){
	int ret = 0;
	FILE *fp = NULL;
	struct minitos_slist_head *mslist_head = &routeshow->slist_head[MTOS_SLIST_FOR_KERNEL];

	while(fgets(read_buf, sizeof(read_buf), fp)){
		memset(&mroute, 0, sizeof(mroute));
		ret = mtos_format_mroute_by_kernelshow(&mroute, read_buf, flag);
		if(ret < 0)
			continue;
		mroute.flag |= MINITOS_FLAG_PROTO_KERNEL;
		
		//ret = minitos_format_filter_mroute(routeshow, &mroute);
		ret = routeshow->func.ftmroute(routeshow, &mroute);
		if(ret < 0){
			ret = 0;
			continue;
		}
		minitos_addmroute2showlist(mslist_head, &mroute, NULL, 0);
	}

end:
	if(fp)
		pclose(fp);
	return 0;
}

int mtos_getroute2kernel(struct minitos_routeshow *routeshow){
	int ret = 0;
	unsigned int flag = routeshow->filter_route.flag;
	if(flag & MINITOS_FLAG_SHOW_ALL){
		ret = mtos_getroute2kernel_ipv(routeshow, MINITOS_IPV4);
	}
	if(flag & MINITOS_FLAG_SHOW_ALL){
		ret = mtos_getroute2kernel_ipv(routeshow, MINITOS_IPV6);
	}
end:
	return ret;
}

int mtos_route_init_showlist(struct minitos_routeshow *routeshow){
	int ret = 0;
	//get route from local save
	if(routeshow->slist_head[MTOS_SLIST_FOR_USER].is_init){
		ret = mtos_getroute2local(routeshow, NULL,  minitos_addmroute2showlist); // 依赖反转 遍历执行每个OBJ 分配一个showlist挂到 回调对象 的user_list中
	}
	if(routeshow->slist_head[MTOS_SLIST_FOR_KERNEL].is_init){ // 内核每一行初始化成一个route 挂到kernel_list上
		ret = mtos_getroute2kernel(routeshow);
	}
	//get family flag from kernel
	ret = mtos_getroute_flag2kernel(&routeshow->slist_head[MTOS_SLIST_FOR_USER]);
end:
	return ret;
}

static void	minitos_add_routeflag(struct minitos_showlist *pos){
	int dst_flag = pos->mroute.dst.flag;
	int gw_flag = pos->mroute.gw.flag;
	int prefix = pos->mroute.dst.subnet;
	int metric = pos->mroute.metric;
	unsigned int mroute_flag = pos->mroute.flag;
	//add G flag	
	if(minitos_configgw_route(mroute_flag)){
		pos->flag |= MINITOS_FLAG_ROUTE_GW;	
	}

	//add H flag
	if(minitos_is_host_route(prefix, dst_flag)){
		pos->flag |= MINITOS_FLAG_ROUTE_HOST;	
	}

	//add i flag
	if(minitos_configdev_route(mroute_flag)){
		pos->flag |= MINITOS_FLAG_ROUTE_IN;
	}
	
	//add S flag
	if(minitos_is_static_route(metric, dst_flag)){
		pos->flag |= MINITOS_FLAG_ROUTE_STATIC;	
	}
	
	//add L flag
	if(minitos_is_lo_route(metric, prefix, dst_flag)){
		pos->flag |= MINITOS_FLAG_ROUTE_UP;	
	}
	
	//add C flag
	if(minitos_is_conn_route(metric, prefix, dst_flag)){
		pos->flag |= MINITOS_FLAG_ROUTE_CONN;
	}

	return;
}

static void minitos_showflag_i2s(unsigned int flag, char *show_sflag, size_t size){
	size_t len = 0;
	if(flag & MINITOS_FLAG_ROUTE_UP)
		len += snprintf(show_sflag + len, size - len,"%s", MINITOS_FLAG_ROUTE_UP_STR);
	if(flag & MINITOS_FLAG_ROUTE_STATIC)
		len += snprintf(show_sflag + len, size - len,"%s", MINITOS_FLAG_ROUTE_STATIC_STR);
	if(flag & MINITOS_FLAG_ROUTE_HOST)
		len += snprintf(show_sflag + len, size - len,"%s", MINITOS_FLAG_ROUTE_HOST_STR);
	if(flag & MINITOS_FLAG_ROUTE_CONN)
		len += snprintf(show_sflag + len, size - len,"%s", MINITOS_FLAG_ROUTE_CONN_STR);
	if(flag & MINITOS_FLAG_ROUTE_IN)
		len += snprintf(show_sflag + len, size - len,"%s", MINITOS_FLAG_ROUTE_IN_STR);
	if(flag & MINITOS_FLAG_ROUTE_GW)
		len += snprintf(show_sflag + len, size - len,"%s", MINITOS_FLAG_ROUTE_GW_STR);
	return ;
}
void mtos_format_oneroute_for_snmp(struct minitos_route *pmroute, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, char *show_buf, size_t max_len, size_t *format_len){
	int i = 0;
	route_talbe_mib	table_mib;
	long long subnet = 0;
	unsigned int dst_h2n = 0;
	unsigned int dst_tmp = 0;
	memset(&table_mib, 0, sizeof(table_mib));

	//family
	if(pmroute->dst.flag & MINITOS_IPV4){
		table_mib.family = (char)MINITOS_SNMP_IPV4;
	}else if (pmroute->dst.flag & MINITOS_IPV6){
		table_mib.family = (char)MINITOS_SNMP_IPV6;
	}else{
		goto end;	
	}
	
	//rt_dst
	memcpy(&table_mib.rt_dst, &pmroute->dst.addr_ip, sizeof(table_mib.rt_dst));

	dst_tmp = table_mib.rt_dst[0];
	dst_h2n = htonl(dst_tmp);
	table_mib.rt_dst[0] = dst_tmp;

	//rt_gateway
	if(!(pmroute->flag & MINITOS_FLAG_NONE_GW)){
		memcpy(&table_mib.rt_gateway, &pmroute->gw.addr_ip, sizeof(table_mib.rt_gateway));
	}

	//rt_flags
	if(!strstr(show_sflag, MINITOS_FLAG_ROUTE_UP_STR)){
		table_mib.rt_flags = MINITOS_SNMP_NO_UP;
	}else{
		if(strstr(show_sflag, MINITOS_FLAG_ROUTE_CONN_STR))
			table_mib.rt_flags = MINITOS_SNMP_CONN;
		else
			table_mib.rt_flags = MINITOS_SNMP_NO_CONN;
	}
	table_mib.rt_tos_flags = 1;

	//interface_name
	//if(pmroute->flag & MINITOS_FLAG_NONE_DEV){
	strncpy(table_mib.interface_name, pmroute->dev, sizeof(table_mib.interface_name));
	//}

	//metric 
	table_mib.rt_metric = (short)pmroute->metric;
	
	if(pmroute->dst.subnet > 128){
		goto end;	
	}
	//rt_genmask
	for(i = 0;i < pmroute->dst.subnet; i++){
		subnet |= (1<<(127 - i));
	}

	//route id 
	table_mib.route_id = id;
	
	memcpy(&table_mib.rt_genmask, &subnet, sizeof(table_mib.rt_genmask));
	memcpy(show_buf + *format_len, &table_mib, sizeof(table_mib));

	*format_len += sizeof(table_mib);
end:
	return;
}

//fmat1route
void mtos_format_oneroute_for_save(struct minitos_route *pmroute, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, char *show_buf, size_t max_len, size_t *format_len){

	char oneroute_buf[ROUTE_CONFIG_MAX_LEN_V6] = {0};
	size_t len = 0;
	if(dst && strlen(dst)){
		len += snprintf(oneroute_buf + len, sizeof(oneroute_buf) -len, \
				"%s %s %s ", "network route add", "dst", dst);
	}else
		goto end;

	if(!(pmroute->flag & MINITOS_FLAG_NONE_GW) && gw && strlen(gw)){
		len += snprintf(oneroute_buf + len, sizeof(oneroute_buf) -len,		\
				"%s %s ", "gw", gw);
	}

	if(!(pmroute->flag & MINITOS_FLAG_NONE_DEV) && strlen(pmroute->dev)){
		len += snprintf(oneroute_buf + len, sizeof(oneroute_buf) - len,		\
				"%s %s ", "dev", pmroute->dev);
	}

	len += snprintf(oneroute_buf + len, sizeof(oneroute_buf) - len,		\
			"%s %u ", "metric", pmroute->metric);

	len += snprintf(oneroute_buf + len, sizeof(oneroute_buf) - len,		\
			"%c", '\n');

	*format_len += snprintf(show_buf + *format_len, max_len - *format_len, "%s",oneroute_buf);


	ROUTE_DEBUG("the save buf is %s", oneroute_buf);
end:
	return;
	
}
//fmat1route
void mtos_format_oneroute_for_webui(struct minitos_route *pmroute, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, char *show_buf, size_t max_len, size_t *len){
	char *s_dev = pmroute->dev;
	unsigned int metric = pmroute->metric;
	//format dst
	*len += snprintf(show_buf + *len, max_len - *len - 1, "%s", dst);
	show_buf[(*len)++] = '\0';

	//format gw
	*len += snprintf(show_buf + *len, max_len - *len - 1, "%s", gw);
	show_buf[(*len)++] = '\0';

	//format metric
	*len += snprintf(show_buf + *len, max_len - *len - 1, "%u", metric);
	show_buf[(*len)++] = '\0';

	//format weight
	mtos_wformat_weigth(show_buf, max_len, len);
	show_buf[(*len)++] = '\0';

	//format dev
	*len += snprintf(show_buf + *len, max_len - *len - 1, "%s", s_dev);
	show_buf[(*len)++] = '\0';
	
	//format search id
	mtos_wformat_search_id(show_buf, max_len, len);
	show_buf[(*len)++] = '\0';
	
	//format flags
	*len += snprintf(show_buf + *len, max_len - *len - 1, "%s", show_sflag);
	show_buf[(*len)++] = '\0';

	//format vr
	mtos_wformat_vr(show_buf, max_len, len);
	show_buf[(*len)++] = '\0';
	
	//format option
	if(strstr(show_sflag, MINITOS_FLAG_ROUTE_STATIC_STR)){
		*len += snprintf(show_buf + *len, max_len - *len - 1, "%s", "Y");
	}else{
		*len += snprintf(show_buf + *len, max_len - *len - 1, "%s", "N");
	}
	show_buf[(*len)++] = '\0';

	//format family
	if(pmroute->dst.flag & MINITOS_IPV4){
		*len += snprintf(show_buf + *len, max_len - *len - 1, "%s", "ipv4");
	}else{
		*len += snprintf(show_buf + *len, max_len - *len - 1, "%s", "ipv6");
	}
	show_buf[(*len)++] = '\0';

	//format route id
	*len += snprintf(show_buf + *len, max_len - *len - 1, "%u", id);
	show_buf[(*len)++] = '\0';
	return;
}

//fmat1route
void mtos_format_oneroute_for_cli(struct minitos_route *pmroute, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, char *show_buf, size_t max_len, size_t *len){
	char *s_dev = pmroute->dev;
	unsigned int metric = pmroute->metric;

	if(pmroute->dst.flag & MINITOS_IPV4){
		*len += snprintf(show_buf + *len, max_len - *len, "%u\t%-24s%-24s%-8s%-8u%-8s\n",	\
				id, dst, gw, show_sflag, metric, s_dev);
	}else{
		*len += snprintf(show_buf + *len, max_len - *len, "%u\t%-40s%-40s%-8s%-8u%-8s\n",	\
				id, dst, gw, show_sflag, metric, s_dev);
	}
	return ;
}

void mtos_format_oneroute_for_iproute(struct minitos_route *pmroute, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, char *show_buf, size_t max_len, size_t *len){
	int ret = 0;
	char cmd_line[CMD_LENGTH] = {0};
	unsigned int option = 0;
	if(strstr(show_sflag, MINITOS_FLAG_ROUTE_UP_STR) && \
			strstr(show_sflag, MINITOS_FLAG_ROUTE_ADD)){
			goto end;
	}

	if(!strstr(show_sflag, MINITOS_FLAG_ROUTE_UP_STR) &&	\
			strstr(show_sflag, MINITOS_FLAG_ROUTE_DEL)){
		goto end;
	}

	if(strstr(show_sflag, MINITOS_FLAG_ROUTE_ADD))
		option = IP_ROUTE_ADD;
	else if(strstr(show_sflag, MINITOS_FLAG_ROUTE_DEL))
		option = IP_ROUTE_DEL;
	else
		goto end;

	ret = minitos_format_cmd(pmroute, option, cmd_line, sizeof(cmd_line));
	if(ret < 0)
		goto end;

	*len += snprintf(show_buf + *len, max_len - *len, "%s 2>/dev/null;", cmd_line);
end:
	return;
}

void mtos_format_oneroute_for_netlink(struct minitos_route *pmroute, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, char *show_buf, size_t max_len, size_t *len){
	mtos_format_oneroute_for_iproute(pmroute, id, dst, gw, show_sflag, show_buf, max_len, len);
}

void mtos_format_oneroute_for_delete(struct minitos_route *pmroute, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, char *show_buf, size_t max_len, size_t *len){
	mtos_format_oneroute_for_iproute(pmroute, id, dst, gw, show_sflag, show_buf, max_len, len);
	minitos_route2configd(MTOS_DEL2CONFIGD, NULL, id, NULL, NULL, 0);
}

int mtos_format1route_elemt_for_show(struct minitos_showlist *showlist, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len){

	struct minitos_showlist *pos = showlist;
	char sipaddr[MIDDLE_LENGTH] = {0};
	int ret = 0;

	ret = minitos_format_saddr(&pos->mroute.dst, sipaddr, sizeof(sipaddr));
	if(ret < 0){
		ROUTE_DEBUG("format dst error");
		goto end;
	}
	//format dst
	snprintf(dst, buf_size, "%s/%u", sipaddr, pos->mroute.dst.subnet);
	if(minitos_is_default_route(dst, buf_size, pos->mroute.dst.flag)){
		 /* in ip route show cmd ,the default dst is "default" not is
		  * "0.0.0.0/0"
		  */
		strncpy(dst, MINITOS_ROUTE_DEFAULT_STR, buf_size);
	}

	//format gw
	if(pos->mroute.flag & MINITOS_FLAG_NONE_GW ){
		strncpy(gw, ((pos->mroute.dst.flag & MINITOS_IPV4) ?	\
					MINITOS_NONE_GWIPv4_STR : MINITOS_NONE_GWIPv6_STR), buf_size);
	}else{
		ret = minitos_format_saddr(&pos->mroute.gw, gw, buf_size);
		if(ret < 0){
			ROUTE_DEBUG("format gw error");
			goto end;
		}
	}

	minitos_add_routeflag(pos);
	minitos_showflag_i2s(pos->flag, show_sflag, buf_size);
end:
	return 0;
}

int mtos_format1route_elemt_for_webui(struct minitos_showlist *showlist, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len){
	int ret = 0;
	ret = mtos_format1route_elemt_for_show(showlist, id, dst, gw,	\
			show_sflag, buf_size, show_buf, max_len, len);
	return 0;
}

int mtos_format1route_elemt_for_cli(struct minitos_showlist *showlist, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len){
	int ret = 0;
	ret = mtos_format1route_elemt_for_show(showlist, id, dst, gw,	\
			show_sflag, buf_size, show_buf, max_len, len);
	return 0;
}

int mtos_format1route_elemt_for_netlink_add(struct minitos_showlist *showlist, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len){

	minitos_add_routeflag(showlist);
	minitos_showflag_i2s(showlist->flag, show_sflag, buf_size);

	snprintf(show_sflag + strlen(show_sflag), buf_size - strlen(show_sflag), "%s", MINITOS_FLAG_ROUTE_ADD);

	return 0;
}

int mtos_format1route_elemt_for_netlink_del(struct minitos_showlist *showlist, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len){

	minitos_add_routeflag(showlist);
	minitos_showflag_i2s(showlist->flag, show_sflag, buf_size);

	snprintf(show_sflag + strlen(show_sflag), buf_size - strlen(show_sflag), "%s", MINITOS_FLAG_ROUTE_DEL);
	return 0;
}

int mtos_format1route_elemt_for_delete(struct minitos_showlist *showlist, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len){
	snprintf(show_sflag + strlen(show_sflag), buf_size - strlen(show_sflag), "%s%s",	\
			MINITOS_FLAG_ROUTE_UP_STR, MINITOS_FLAG_ROUTE_DEL);
	return 0;
}

int mtos_format1route_elemt_for_snmp(struct minitos_showlist *showlist, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len){
	minitos_add_routeflag(showlist);
	minitos_showflag_i2s(showlist->flag, show_sflag, buf_size);

	return 0;
}

int mtos_format1route_elemt_for_save(struct minitos_showlist *showlist, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len){
	char sipaddr[MIDDLE_LENGTH] = {0};
	struct minitos_showlist *pos = showlist;
	int ret = 0;
	
	ret = minitos_format_saddr(&pos->mroute.dst, sipaddr, sizeof(sipaddr));
	if(ret < 0){
		ROUTE_DEBUG("format dst error");
		goto end;
	}
	//format dst
	snprintf(dst, buf_size, "%s/%u", sipaddr, pos->mroute.dst.subnet);
	ret = minitos_format_saddr(&pos->mroute.gw, gw, buf_size);
	if(ret < 0){
		ROUTE_DEBUG("format gw error");
		goto end;
	}

end:
	return ret;
}

void mtos_format_showbuf_ipv(struct minitos_routeshow *routeshow, char *show_buf, size_t *len,	\
		size_t max_len,	struct list_head *list_head){
	int ret = 0;
	struct minitos_showlist *pos = NULL;
	struct minitos_showlist *n = NULL;
	
	char dst[MIDDLE_LENGTH] = {0};
	char dev[MIDDLE_LENGTH] = {0};
	char gw[MIDDLE_LENGTH] = {0};
	char show_sflag[MIDDLE_LENGTH] = {0};
	unsigned int metric = 0;
	char *s_dev = NULL;
	struct minitos_slist_func *func = &routeshow->func;
	unsigned int type = routeshow->type;

	struct minitos_route  *filter_mroute = &routeshow->filter_route;

	if(route_list_empty(list_head)){
		goto end;	
	}

	list_for_each_entry_safe(pos, n, list_head, family_list){
		ret = func->fmat1route_elmt(pos, pos->id, dst, gw, \
				show_sflag, MIDDLE_LENGTH, show_buf, max_len, len);
		if(ret < 0){
			ret = 0;
			continue;
		}

		if(!(filter_mroute->flag & MINITOS_FLAG_NONE_DEV) && pos->mroute.flag & MINITOS_FLAG_NONE_DEV){
			if(strncmp(filter_mroute->dev, pos->mroute.dev, sizeof(filter_mroute->dev))){
				if(routeshow->type == MTOS_SHOW_FOR_NETLINK){
					if(strlen(pos->mroute.dev))	
						continue;
				}else
					continue;
			}
		}
#if 1 //WAF_NGTOS_PATCH
		else {
			if((filter_mroute->flag & MINITOS_FLAG_NONE_DEV) && pos->mroute.flag & MINITOS_FLAG_NONE_DEV){
				if (filter_mroute->ifindex != pos->mroute.ifindex) {
					ROUTE_DEBUG("skip unmatch nobind dev routing modified by du_xiaoyu\n");
					continue;
				}

			}
		}
#endif

		func->fmat1route(&pos->mroute, pos->id, dst, gw, show_sflag, show_buf, max_len, len);
	}
end:
	return ;
}
//getlen
size_t mtos_get_showbuf_len(struct minitos_routeshow *routeshow){
	size_t buf_size = 0;
	int i = 0;
	unsigned int ipv4route_num = 0;
	unsigned int ipv6route_num = 0;
	unsigned int type= routeshow->type;
	
	for_each_showlist(i){
		if(routeshow->slist_head[i].is_init == 0)
			continue;
		ipv4route_num += routeshow->slist_head[i].ipv4list_num;
		ipv6route_num += routeshow->slist_head[i].ipv6list_num;
	}

	switch(type){
		case MTOS_SHOW_FOR_CLI:
			buf_size = (ipv4route_num + ((ipv4route_num) ? 3 : 0)) * ROUTE_CONFIG_MAX_LEN_V4 +	\
				(ipv6route_num + ((ipv6route_num) ? 2 : 0)) * ROUTE_CONFIG_MAX_LEN_V6;
			break;
		case MTOS_SHOW_FOR_SNMP:
			buf_size = (ipv4route_num + ipv6route_num) * sizeof(route_talbe_mib);
			break;
		default:
			buf_size = ipv4route_num * ROUTE_CONFIG_MAX_LEN_V4 +	\
				ipv6route_num  * ROUTE_CONFIG_MAX_LEN_V6;
			break;
	}
end:
	return buf_size;
}

void mtos_format_show_for_gen(struct minitos_routeshow *routeshow, size_t max_len, char *showbuf, size_t *buflen){
	int i = 0;
	size_t format_len = 0;
	for_each_showlist(i){
		if(routeshow->slist_head[i].is_init == 0)
			continue;
		mtos_format_showbuf_ipv(routeshow, showbuf, &format_len, max_len, &routeshow->slist_head[i].ipv4_list);
		mtos_format_showbuf_ipv(routeshow, showbuf, &format_len, max_len, &routeshow->slist_head[i].ipv6_list);
	}
	*buflen = format_len;
	return;
}

void mtos_format_showbuf_for_webui(struct minitos_routeshow *routeshow, size_t max_len, char *showbuf, size_t *buflen){
	mtos_format_show_for_gen(routeshow, max_len, showbuf, buflen);
	return;
}

//fshowbuf
void mtos_format_showbuf_for_save(struct minitos_routeshow *routeshow, size_t max_len, char *showbuf, size_t *buflen){
	mtos_format_show_for_gen(routeshow, max_len, showbuf, buflen);
	return;
}
void mtos_format_showbuf_for_netlink(struct minitos_routeshow *routeshow, size_t max_len, char *showbuf, size_t *buflen){
	mtos_format_show_for_gen(routeshow, max_len, showbuf, buflen);
	return;
}

void mtos_format_showbuf_for_snmp(struct minitos_routeshow *routeshow, size_t max_len, char *showbuf, size_t *buflen){
	mtos_format_show_for_gen(routeshow, max_len, showbuf, buflen);
	return;
}

void mtos_format_showbuf_for_delete(struct minitos_routeshow *routeshow, size_t max_len, char *showbuf, size_t *buflen){
	mtos_format_show_for_gen(routeshow, max_len, showbuf, buflen);
	return;
}

//fshowbuf
void mtos_format_showbuf_for_cli(struct minitos_routeshow *routeshow, size_t max_len, char *showbuf, size_t *buflen){

	size_t ipv4route_num = 0;
	size_t ipv6route_num = 0;
	size_t format_len = 0;
	int i = 0;

	for_each_showlist(i){
		if(routeshow->slist_head[i].is_init == 0)
			continue;
		ipv4route_num += routeshow->slist_head[i].ipv4list_num;
		ipv6route_num += routeshow->slist_head[i].ipv6list_num;
	}

	if(ipv4route_num){
		format_len += snprintf(showbuf + format_len , max_len - format_len,	\
				"%s\n","Flags: U-Up, G-Gateway, H-Host, S-Static, C-Connected, i-Interface");
		//========format ipv4==============
		format_len += snprintf(showbuf + format_len , max_len - format_len,	\
				"============================IPv4=============================\n");
		format_len +=  snprintf(showbuf + format_len , max_len - format_len,	\
				"%s\t%-24s%-24s%-8s%-8s%-8s\n",	"id","Destination","Gateway","Flags","Metric","Iface");
		for_each_showlist(i){
			if(routeshow->slist_head[i].is_init == 0)
				continue;
			mtos_format_showbuf_ipv(routeshow, showbuf, &format_len, max_len, &routeshow->slist_head[i].ipv4_list);
		}
	}

	if(ipv6route_num){
		format_len += snprintf(showbuf + format_len , max_len - format_len,	\
				"============================IPv6=============================\n");
		format_len +=  snprintf(showbuf + format_len , max_len - format_len,	\
				"%s\t%-40s%-40s%-8s%-8s%-8s\n", "id","Destination","Gateway","Flags","Metric","Iface");
		for_each_showlist(i){
			if(routeshow->slist_head[i].is_init == 0)
				continue;
			mtos_format_showbuf_ipv(routeshow, showbuf, &format_len, max_len, &routeshow->slist_head[i].ipv6_list);
		}
	}
	*buflen = format_len;
	return;
}

void mtos_format_webui_showbuf(struct minitos_slist_head *mslist_head_user,	\
		struct minitos_slist_head *mslist_head_kernel, size_t max_len, char *showbuf){
	return;	
}
void mtos_format_save_showbuf(struct minitos_slist_head *mslist_head_user,	\
		size_t max_len, char *showbuf){
	return;
}

int mtos_get_and_format_showbuf(struct minitos_routeshow *routeshow, 
		int *buflen, char **buf) {
	int ret = 0;
	int vsid = 0;
	char *showbuf = NULL;
	size_t max_len = 0;
	struct minitos_slist_func *func = &routeshow->func;
	size_t format_len = 0;
	
	//get max_len
	//max_len = func->getlen(routeshow);
	max_len = mtos_get_showbuf_len(routeshow);
	showbuf = (char *)malloc(max_len);
	if(!showbuf){
		*buf = NULL;
		ret = -RT_MALLOC;
		goto end;
	}
	memset(showbuf, 0, max_len);
	*buf = showbuf;

	//format show buf by type
	func->fshowbuf(routeshow, max_len, showbuf, &format_len);
	*buflen = format_len;
end:
	return ret;
}

int route_show_ui(const manager_info_t * minfo, int argc, char **argv, int *buflen, char **buf){
	int ret = 0;
	struct minitos_routeshow   routeshow;
	struct minitos_slist_head *mslist_head_user = &routeshow.slist_head[MTOS_SLIST_FOR_USER];
	struct minitos_slist_head *mslist_head_kernel = &routeshow.slist_head[MTOS_SLIST_FOR_KERNEL];

	mtos_init_webui_routeshow(&routeshow);
	minitos_format_mroute_filter(&routeshow.filter_route, argv[RT_PARAM_SHOW_TYPE],	\
			NULL, NULL, NULL, NULL);

	ret = mtos_route_init_showlist(&routeshow);
	if(ret < 0){
		ROUTE_DEBUG("minitos get local route error");
		goto error;
	}
	ret = mtos_get_and_format_showbuf(&routeshow, buflen, buf);
	if(ret < 0){
		ROUTE_DEBUG("get and format show buf error");
		goto error;
	}

end:
	minitos_mslist_free(mslist_head_user);
	minitos_mslist_free(mslist_head_kernel);
	return ret;
error:
	if(*buf){
		free(*buf);
		*buf = NULL;
		*buflen = 0;
	}
	goto end;
}

int route_show_for_snmp(char **buf, size_t *buflen)
{
	unsigned int vsid = 0;
	int ret = 0;
	struct minitos_routeshow   routeshow;
	struct minitos_slist_head *mslist_head_user = &routeshow.slist_head[MTOS_SLIST_FOR_USER];
	struct minitos_slist_head *mslist_head_kernel = &routeshow.slist_head[MTOS_SLIST_FOR_KERNEL];
	mtos_init_snmp_routeshow(&routeshow);

	routeshow.filter_route.flag |= MINITOS_FLAG_SHOW_ALL;

	if(ret < 0){
		ROUTE_DEBUG("format mroute filter error");
		goto error;
	}

	ret = mtos_route_init_showlist(&routeshow);

	if(ret < 0){
		ROUTE_DEBUG("format show list error");
		goto error;
	}
	
	ret = mtos_get_and_format_showbuf(&routeshow, (int*)buflen, buf);
	if(ret < 0)
		goto error;
end:
	minitos_mslist_free(mslist_head_user);
	minitos_mslist_free(mslist_head_kernel);
	return ret;
error:
	if(*buf){
		free(*buf);
		*buf = NULL;
		*buflen = 0;
	}
	goto end;
}

int route_save(const manager_info_t * minfo, int argc, char **argv, int *buflen, char **buf){
	int ret = 0;
	struct minitos_routeshow   routeshow;
	struct minitos_slist_head *mslist_head_user = &routeshow.slist_head[MTOS_SLIST_FOR_USER];

	mtos_init_save_routeshow(&routeshow);
	minitos_format_mroute_filter(&routeshow.filter_route, NULL, NULL, NULL, NULL, NULL);

	ret = mtos_route_init_showlist(&routeshow);

	if(ret < 0){
		ROUTE_DEBUG("minitos init showlist error");
		goto error;
	}

	ret = mtos_get_and_format_showbuf(&routeshow, buflen, buf);

end:
	minitos_mslist_free(mslist_head_user);
	return ret;
error:
	if(*buf){
		free(*buf);
		*buf = NULL;
		*buflen = 0;
	}
	goto end;
}

int route_del_id(const manager_info_t * minfo, int argc, char **argv, int *buflen, char **buf)
{	
	int ret = 0;
	int id = 0;
	char *p_id = argv[0];
	unsigned int vsid = minfo->vsid;
	struct minitos_route mroute;
	
	if(!p_id || !strlen(p_id)){
		goto end;
	}

	id = atoi(p_id);
	if(id == 0){
		ret = -RT_EID;
		goto end;
	}

	ret = minitos_showroute_by_id2configd(&mroute, id, vsid);
	if(ret < 0){
		ROUTE_DEBUG("get route from configd error");
		ret = -RT_ENOENTRY;
		goto end;
	}

	ret = minitos_del_route2kerel(&mroute);
	if(ret < 0){
		if(ret == -RT_EEXIST){
			ret = 0;	
			ROUTE_DEBUG("this route can't be not effect");
		}else{
			ROUTE_DEBUG("del route2kernel error");
			goto end;
		}
	}
	ret = minitos_del_route2configd(id, vsid);
end:
	return ret;
}

int route_del_trans_dstgw(const manager_info_t *minfo, char **argv, char *dst,	\
		size_t dst_len, char **gw){
	char *pdst = argv[RT_PARAM_DEL_DST];
	char *pgw = argv[RT_PARAM_DEL_GW];
	char *pfamily = argv[RT_PARAM_DEL_AF];
	int ret = 0;
	if(minfo->m_type == M_TYPE_WEBUI){
		if(!pdst || !pgw || !pfamily){
			ret = -RT_ETYPE;	
			goto end;
		}
		if(!strcmp(pdst, MINITOS_ROUTE_DEFAULT_STR)){
			strncpy(dst, (!strcmp(pfamily, MINITOS_ROUTE_IPV4_FA_STR)) ?	\
					MINITOS_ROUTE_DEFAULT_IPV4 : MINITOS_ROUTE_DEFAULT_IPV6, dst_len);
		}else{
			strncpy(dst, pdst, dst_len);
		}

		if(!strcmp(pgw, MINITOS_NONE_GWIPv4_STR) || !strcmp(pgw, MINITOS_NONE_GWIPv6_STR)){
			*gw = NULL;
		}else{
			*gw = pgw;	
		}
	}else{
		if(!pdst){
			ret = -RT_ETYPE;	
			goto end;
		}
		strncpy(dst, pdst, dst_len);
		*gw = pgw;
	}
end:
	return ret;
}

///////////////////////////////////增删 没有改///////////////////////////////////////////////////////////////////////
static int minitos_route2configd(unsigned int option, struct minitos_route *mroute, 
		int id, char **databuf, size_t *len, unsigned int vsid) {
	int ret = 0;
	int size = 0;
	size_t obj_len = 0;
	struct minitos_route  *route_ptr;
	struct tos_obj_head *obj = NULL;

	size = sizeof(struct minitos_route);

	switch(option){
		case MTOS_ADD2CONFIGD:
			ret = add_obj_vs(OBJ_TYPE_MINITOS_ROUTE, NULL, 0, NULL, size, (char *) mroute, 0);
			if(ret < 0){
				ROUTE_DEBUG("add route obj by vs id, ret = %d \n", ret);
				goto end;
			}else{
				ROUTE_DEBUG("add a new route \n");
			}
			break;
		case MTOS_DEL2CONFIGD:
			ret = delete_obj_by_ID_vs(OBJ_TYPE_MINITOS_ROUTE, id, 0);
			if(ret < 0){
				ROUTE_DEBUG("del route obj by vs id, ret = %d \n", ret);
				goto end;
			}
			break;
		case MTOS_GET_BY_ID2CONFIGD:
			ret = get_obj_by_ID_vs(OBJ_TYPE_MINITOS_ROUTE, id, (char **)&obj, (__u32 *) &obj_len, 0);
			if(ret < 0){
				ROUTE_DEBUG("get route obj by vs id, ret = %d \n", ret);
				goto end;
			}
			route_ptr = (struct minitos_route *)((char *)obj + sizeof(struct tos_obj_head));
			memcpy(mroute, route_ptr, sizeof(struct minitos_route));
			break;
		case MTOS_GET_BY_VS2CONFIGD:
			ret = get_obj_by_type_vs(OBJ_TYPE_MINITOS_ROUTE, databuf, (__u32 *)len, vsid);
			if(ret < 0){
				ROUTE_DEBUG("get route obj by vs error, ret = %d \n", ret);
				goto end;
			}
			break;
		case MTOS_GET_NUM2CONFIGD:
			ret = get_num_by_type_vs(OBJ_TYPE_MINITOS_ROUTE, vsid);
			if(ret < 0){
				ROUTE_DEBUG("get route num by vs error, ret = %d \n", ret);
				goto end;
			}
			break;
		case MTOS_GET_EXIST_BY_ID:
			ret = get_obj_by_ID_vs(OBJ_TYPE_MINITOS_ROUTE, id, (char **)&obj, (__u32 *) &obj_len, vsid);
			if(ret < 0){
				ret = -RT_ENOENTRY;
			}	
			if(obj){
				free(obj);
			}
			break;
		default:
			ret = -1;
			goto end;
	}

end:
	if(obj){
		obj = NULL;
		free(obj);
	}
	return ret;
}

static int minitos_add_route2configd(struct minitos_route *mroute, unsigned int vsid) {
	return minitos_route2configd(MTOS_ADD2CONFIGD, mroute, 0, NULL, NULL, vsid);
}

static int minitos_del_route2configd(int id, unsigned int vsid) {
	return minitos_route2configd(MTOS_DEL2CONFIGD, NULL, id, NULL, NULL, vsid);
}

static int minitos_showroute_by_id2configd(struct minitos_route *mroute, int id, unsigned int vsid){
	return minitos_route2configd(MTOS_GET_BY_ID2CONFIGD, mroute, id, NULL, NULL, vsid);
}

static int minitos_showroute_by_vs2configd(char **data, unsigned int vsid, size_t *len){
	return minitos_route2configd(MTOS_GET_BY_VS2CONFIGD, NULL, 0, data, len, vsid);
}

static int minitos_showroute_num2configd(unsigned int vsid){
	return minitos_route2configd(MTOS_GET_NUM2CONFIGD, NULL, 0, NULL, NULL, vsid);
}

static int minitos_showroute_exist2configd(int id, unsigned int vsid){
	return minitos_route2configd(MTOS_GET_EXIST_BY_ID, NULL, id, NULL, NULL, vsid);
}



