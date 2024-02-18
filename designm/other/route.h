#define IP_TYPE_V4	0
#define IP_TYPE_V6	1
#define ROUTE_CONFIG_MAX_LEN_V4		96
#define ROUTE_CONFIG_MAX_LEN_V6		160
#define ROUTE_MATRIC_MAX			32766
#define ROUTE_V4_MATRIC_MIN			1
#define ROUTE_V6_MATRIC_MIN			257
#define ROUTE_CONFIG_NUM_MAX		1000

#define ROUTE_V4_PREFIX_MAX			32
#define ROUTE_V6_PREFIX_MAX			128

#define IP_ROUTE_HEAD_SIZE			16
#define IP_ROUTE_OPTION_SIZE		8
#define ROUTE_MASKIPV6_MAX			2 

#define IP_ROUTE_GW					"via"
#define IP_ROUTE_DEV				"dev"
#define IP_ROUTE_METRIC				"metric"

#define IP_ROUTE_PROTO_KERNEL		"proto kernel"

#define MINITOS_ROUTE_DEFAULT_IPV4		"0.0.0.0/0"
#define MINITOS_ROUTE_DEFAULT_IPV6		"::/0"
#define MINITOS_ROUTE_DEFAULT_STR		"default"
#define MINITOS_NONE_GWIPv4_STR				"*"
#define MINITOS_NONE_GWIPv6_STR				"::"
#define MINITOS_ROUTE_IPV4_FA_STR			"ipv4"
#define MINITOS_ROUTE_IPV6_FA_STR			"ipv6"


#define MINITOS_SNMP_IPV4			2
#define MINITOS_SNMP_IPV6			10

#define MINITOS_SNMP_NO_UP			2
#define MINITOS_SNMP_CONN			3
#define MINITOS_SNMP_NO_CONN		4

#ifndef offsetof
#define offsetof(type, member) (size_t)&(((type*)0)->member)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif


#define mtos_need_change_dst_default_str(type)	\
	((type == MTOS_SHOW_FOR_CLI || type == MTOS_SHOW_FOR_WEBUI) ? 1 : 0)

#define mtos_need_chnange_gw_none_str(type)	\
	((type == MTOS_SHOW_FOR_CLI || type == MTOS_SHOW_FOR_WEBUI) ? 1 : 0)

#define mtos_choose_max_prefix_by_flag(flag)	\
	((flag & MINITOS_IPV4) ? ROUTE_V4_PREFIX_MAX : ROUTE_V6_PREFIX_MAX)

#define mtos_choose_default_dst_by_flag(flag)	\
	((flag & MINITOS_IPV4) ? MINITOS_ROUTE_DEFAULT_IPV4 : MINITOS_ROUTE_DEFAULT_IPV6)

enum {
	RT_PARAM_ADD_DST=0,
	RT_PARAM_ADD_AF,
	RT_PARAM_ADD_GW,
	RT_PARAM_ADD_METRIC,
	RT_PARAM_ADD_DEV,
	RT_PARAM_ADD_MAX
	//RT_PARAM_ADD_ID
};

enum {
	RT_PARAM_DEL_DST=0,
	RT_PARAM_DEL_AF,
	RT_PARAM_DEL_GW,
	RT_PARAM_DEL_METRIC,
	RT_PARAM_DEL_DEV
};

enum {
    RT_PARAM_DEL_ID=0,
};

enum {
	RT_PARAM_CLEAN_FAMILY=0,
//	RT_PARAM_CLEAN_VRID
};

enum {
    RT_PARAM_SETTING_SOURCE=0
};

enum {
    RT_PARAM_SHOW_TYPE=0,
    RT_PARAM_SHOW_DST,
    RT_PARAM_SHOW_GW,
    RT_PARAM_SHOW_DEV,
    RT_PARAM_SHOW_FUNC
};

#define RT_PARAM_SETTING_TRACED    0

enum {
	RT_PARAM_LOOKUP_FAMILY = 0,
	RT_PARAM_LOOKUP_TYPE,
	RT_PARAM_LOOKUP_DST,
	RT_PARAM_LOOKUP_OIF,
	RT_PARAM_LOOKUP_MAX
};

enum{
	IP_ROUTE_CMD_IPV4,
	IP_ROUTE_CMD_IPV6,
	IP_ROUTE_CMD_MAX
}ip_route_cmd_head;

#define get_ip_route_cmd_head(flag)	\
	((flag & MINITOS_IPV4) ? iproute_head[IP_ROUTE_CMD_IPV4] : iproute_head[IP_ROUTE_CMD_IPV6])

enum{
	IP_ROUTE_ADD,
	IP_ROUTE_DEL,
	IP_ROUTE_SHOW,
	IP_ROUTE_OPTION_MAX
}ip_route_option;

enum{
	MTOS_ADD2CONFIGD,
	MTOS_DEL2CONFIGD,
	MTOS_GET_BY_ID2CONFIGD,
	MTOS_GET_BY_VS2CONFIGD,
	MTOS_GET_NUM2CONFIGD,
	MTOS_GET_EXIST_BY_ID,
	MTOS_OPTION2CONFIGD_MAX
}mtos_option2configd;


struct minitos_route{
	struct minitos_addr dst;
	struct minitos_addr gw;
	unsigned int netmask;
	unsigned int metric;
	unsigned int flag;
	char dev[MIDDLE_LENGTH];
#if 1 //WAF_NGTOS_PATCH
	int   ifindex;
#endif
};

//the follow four, may be use for usr filter
#define MINITOS_FLAG_SHOW_ALL		(1<<0)			//usr don't filter by ip family
#define MINITOS_FLAG_SHOW_IPV4		(1<<1)			//usr filter show ipv4 route
#define MINITOS_FLAG_SHOW_IPV6		(1<<2)			//usr filter show ipv6 route
#define MINITOS_FLAG_NONE_DEV		(1<<3)			//usr not configure dev || usr filter not by dev
#define MINITOS_FLAG_NONE_GW		(1<<4)			//usr not configure gw  || usr filter not by gw
#define MINITOS_FLAG_NONE_DST		(1<<5)			//usr filter not by dst
#define MINITOS_FLAG_NONE_METRIC	(1<<6)			//usr filter not by metric
#define MINITOS_FLAG_PROTO_KERNEL	(1<<7)			//'proto kernel' route

struct minitos_showlist{
	unsigned int id;
	unsigned int flag;
	struct minitos_route mroute;
	struct list_head show_list;
	struct list_head family_list;
};

#define MINITOS_FLAG_ROUTE_UP		(1<<0)
#define MINITOS_FLAG_ROUTE_STATIC	(1<<1)
#define MINITOS_FLAG_ROUTE_HOST		(1<<2)
#define MINITOS_FLAG_ROUTE_CONN		(1<<3)
#define MINITOS_FLAG_ROUTE_IN		(1<<4)
#define MINITOS_FLAG_ROUTE_GW		(1<<5)

#define minitos_is_default_route(sdst, size, flag)	\
	(((!strncmp(sdst, MINITOS_ROUTE_DEFAULT_IPV4, size) && MINITOS_IPV4 & flag)||	\
	 (!strncmp(sdst, MINITOS_ROUTE_DEFAULT_IPV6, size) && MINITOS_IPV6 & flag))? 1 : 0)

#define minitos_is_host_route(prefix, flag)		\
	(((flag & MINITOS_IPV4 && prefix == 32) || (flag & MINITOS_IPV6 && prefix == 128)) ? 1 : 0)

#define minitos_is_static_route(metric, flag)		\
	(((flag & MINITOS_IPV4 && metric != 0) || (flag & MINITOS_IPV6 && metric != 256)) ? 1 : 0)

#define minitos_is_lo_route(metric, prefix, flag)		\
	(((flag & MINITOS_IPV4 && metric == 0 && prefix == 32)		\
	   || (flag & MINITOS_IPV6 && metric == 256 && prefix == 128)) ? 1 : 0)

#define minitos_is_conn_route(metric, prefix, flag)		\
	(((flag & MINITOS_IPV4 && metric == 0 && prefix != 32)		\
	   || (flag & MINITOS_IPV6 && metric == 256 && prefix != 128)) ? 1 : 0)

#define minitos_configgw_route(flag)		\
	((flag & MINITOS_FLAG_NONE_GW || (flag & MINITOS_FLAG_PROTO_KERNEL)) ? 0 : 1)

#define minitos_configdev_route(flag)		\
	((flag & MINITOS_FLAG_NONE_DEV && !(flag & MINITOS_FLAG_PROTO_KERNEL)) ? 0 : 1)

#define MINITOS_FLAG_ROUTE_UP_STR			"U"
#define MINITOS_FLAG_ROUTE_STATIC_STR		"S"
#define MINITOS_FLAG_ROUTE_HOST_STR			"H"
#define MINITOS_FLAG_ROUTE_CONN_STR			"C"
#define MINITOS_FLAG_ROUTE_IN_STR			"i"
#define MINITOS_FLAG_ROUTE_GW_STR			"G"

#define MINITOS_FLAG_ROUTE_ADD				"A"
#define MINITOS_FLAG_ROUTE_DEL				"D"
#define route_list_empty(list_head)			\
	((((list_head)->next == NULL || (list_head)->prev == NULL) \
	|| list_empty((list_head))) ? 1 : 0)
struct minitos_slist_head{
	unsigned int is_init;
	unsigned int showlist_num;
	unsigned int ipv4list_num;
	unsigned int ipv6list_num;
	struct list_head show_list;
	struct list_head ipv4_list;
	struct list_head ipv6_list;
	//use for get head
};

struct minitos_slist_func;
struct minitos_routeshow;
typedef void(*format_showbuf)(struct minitos_routeshow *, size_t, char *, size_t *);
typedef void (*format_oneroute)(struct minitos_route *,	unsigned int, char *, char*,	\
		char*, char *, size_t, size_t*);

typedef int(*format_oneroute_elemt)(struct minitos_showlist*,	unsigned int, char *, char*,	\
		char*, size_t, char *, size_t, size_t*);

typedef int(*filter_mroute)(struct minitos_routeshow* routeshow, 
		struct minitos_route *pmroute);

typedef int(*get_from_l2do)(
		struct minitos_slist_head *mslist_head,		\
		struct minitos_route * cmroute,		\
		struct minitos_route * pmroute,		\
		unsigned int id);


struct minitos_slist_func{
	format_showbuf fshowbuf;
	format_oneroute	fmat1route;
	format_oneroute_elemt fmat1route_elmt;
	filter_mroute ftmroute;
};

enum {  
	MTOS_SLIST_FOR_USER,
	MTOS_SLIST_FOR_KERNEL,
	MTOS_SLIST_MAX
};

enum{
	MTOS_SHOW_FOR_CLI = 0,
	MTOS_SHOW_FOR_WEBUI,
	MTOS_SHOW_FOR_SAVE, 
	MTOS_SHOW_FOR_NETLINK,
	MTOS_SHOW_FOR_CLEAN,
	MTOS_SHOW_FOR_SNMP,
	MTOS_SHOW_FOR_DELETE
}mtos_route_show_type;

#define for_each_showlist(i)				\
	for(i = 0; i < MTOS_SLIST_MAX; i++)	

#define mtos_need_change_fuser(type)	\
	((type == MTOS_SHOW_FOR_SAVE) ? 0 : 1)

struct minitos_routeshow{
	unsigned int type;
	unsigned int flag;
	struct minitos_route filter_route;
	struct minitos_slist_head slist_head[MTOS_SLIST_MAX];
	struct minitos_slist_func func;
};

enum{
	MTOS_LIST_HEAD_SHOW = 0,
	MTOS_LIST_HEAD_FA
}mtos_list_head;
#define mtos_init_cli_showfunc(slist_func)									\
	do{																		\
		(slist_func)->fshowbuf = mtos_format_showbuf_for_cli;				\
		(slist_func)->fmat1route= mtos_format_oneroute_for_cli;				\
		(slist_func)->ftmroute= minitos_format_filter_mroute;				\
		(slist_func)->fmat1route_elmt =  mtos_format1route_elemt_for_cli;	\
	}while(0)
#define mtos_init_save_showfunc(slist_func)							\
	do{																\
		(slist_func)->fshowbuf = mtos_format_showbuf_for_save;		\
		(slist_func)->fmat1route = mtos_format_oneroute_for_save;	\
		(slist_func)->ftmroute = minitos_format_filter_mroute_save;		\
		(slist_func)->fmat1route_elmt= mtos_format1route_elemt_for_save;	\
	}while(0)

#define mtos_init_webui_showfunc(slist_func)								\
	do{																		\
		(slist_func)->fshowbuf = mtos_format_showbuf_for_webui;				\
		(slist_func)->fmat1route= mtos_format_oneroute_for_webui;			\
		(slist_func)->ftmroute = minitos_format_filter_mroute;				\
		(slist_func)->fmat1route_elmt= mtos_format1route_elemt_for_webui;	\
	}while(0)

#define mtos_init_snmp_showfunc(slist_func)									\
	do{																		\
		(slist_func)->fshowbuf = mtos_format_showbuf_for_snmp;				\
		(slist_func)->fmat1route= mtos_format_oneroute_for_snmp;			\
		(slist_func)->ftmroute = minitos_format_filter_mroute_snmp;			\
		(slist_func)->fmat1route_elmt= mtos_format1route_elemt_for_snmp;	\
	}while(0)

#define mtos_init_delete_showfunc(slist_func)								\
	do{																		\
		(slist_func)->fshowbuf = mtos_format_showbuf_for_delete;			\
		(slist_func)->fmat1route= mtos_format_oneroute_for_delete;			\
		(slist_func)->ftmroute = minitos_format_filter_mroute;				\
		(slist_func)->fmat1route_elmt= mtos_format1route_elemt_for_delete;	\
	}while(0)

#define mtos_init_netlink_showfunc(slist_func, flag)								\
	do{																				\
		(slist_func)->fshowbuf = mtos_format_showbuf_for_netlink;					\
		(slist_func)->fmat1route= mtos_format_oneroute_for_netlink;					\
		(slist_func)->ftmroute = minitos_format_filter_mroute_netlink;				\
		if(flag == IP_ROUTE_ADD)													\
			(slist_func)->fmat1route_elmt= mtos_format1route_elemt_for_netlink_add;	\
		else																		\
			(slist_func)->fmat1route_elmt= mtos_format1route_elemt_for_netlink_del;	\
	}while(0)

#define mtos_init_clean_showfunc(slist_func)							\
	do{																	\
		(slist_func)->ftmroute = minitos_format_filter_mroute_clean;	\
	}while(0)

#define mtos_init_showlist_num(mslist_head)				\
	do{													\
		(mslist_head)->showlist_num = 0;				\
		(mslist_head)->ipv4list_num = 0;				\
		(mslist_head)->ipv6list_num = 0;				\
	}while(0)

#define mtos_init_showlist_head(mslist_head)			\
	do{													\
		INIT_LIST_HEAD(&(mslist_head)->show_list);		\
		INIT_LIST_HEAD(&(mslist_head)->ipv4_list);		\
		INIT_LIST_HEAD(&(mslist_head)->ipv6_list);		\
	}while(0)

#define mtos_init_showlist(mslist_head)						\
	do{														\
		(mslist_head)->is_init = 1;							\
		mtos_init_showlist_num(mslist_head);				\
		mtos_init_showlist_head(mslist_head);				\
	}while(0)

#define mtos_init_cli_routeshow(routeshow)					\
	do{														\
		memset((routeshow), 0,								\
				sizeof(struct minitos_routeshow));			\
		(routeshow)->type = MTOS_SHOW_FOR_CLI;				\
		mtos_init_showlist(&(routeshow)->						\
					slist_head[MTOS_SLIST_FOR_USER]);		\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_KERNEL]);		\
		mtos_init_cli_showfunc(&(routeshow)->func);			\
	}while(0)

#define mtos_init_netlink_routeshow(routeshow, flag)		\
	do{														\
		memset((routeshow), 0,								\
				sizeof(struct minitos_routeshow));			\
		(routeshow)->type = MTOS_SHOW_FOR_NETLINK;			\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_USER]);		\
		mtos_init_netlink_showfunc(&(routeshow)->func, flag);		\
	}while(0)

#define mtos_init_save_routeshow(routeshow)					\
	do{														\
		memset((routeshow), 0,								\
				sizeof(struct minitos_routeshow));			\
		(routeshow)->type = MTOS_SHOW_FOR_SAVE;				\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_USER]);		\
		mtos_init_save_showfunc(&(routeshow)->func);			\
	}while(0)

#define mtos_init_webui_routeshow(routeshow)					\
	do{														\
		memset((routeshow), 0,								\
				sizeof(struct minitos_routeshow));			\
		(routeshow)->type = MTOS_SHOW_FOR_WEBUI;				\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_USER]);		\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_KERNEL]);		\
		mtos_init_webui_showfunc(&(routeshow)->func);		\
	}while(0)

#define mtos_init_snmp_routeshow(routeshow)					\
	do{														\
		memset((routeshow), 0,								\
				sizeof(struct minitos_routeshow));			\
		(routeshow)->type = MTOS_SHOW_FOR_SNMP;				\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_USER]);		\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_KERNEL]);		\
		mtos_init_snmp_showfunc(&(routeshow)->func);		\
	}while(0)

#define mtos_init_clean_routeshow(routeshow)				\
	do{														\
		memset((routeshow), 0,								\
				sizeof(struct minitos_routeshow));			\
		(routeshow)->type = MTOS_SHOW_FOR_CLEAN;			\
		mtos_init_clean_showfunc(&(routeshow)->func);		\
	}while(0)

#define mtos_init_delete_routeshow(routeshow)				\
	do{														\
		memset((routeshow), 0,								\
				sizeof(struct minitos_routeshow));			\
		(routeshow)->type = MTOS_SHOW_FOR_DELETE;			\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_USER]);		\
		mtos_init_showlist(&(routeshow)->					\
					slist_head[MTOS_SLIST_FOR_KERNEL]);		\
		mtos_init_delete_showfunc(&(routeshow)->func);		\
	}while(0)

#define mtos_fnone_for_webui(buf, max_len, plen) do{}while(0)

#define mtos_fzero_for_webui(buf, max_len, plen)			\
	do{												\
		*(plen) += snprintf(buf + *(plen),			\
				max_len - *(plen) - 1, "%d", 0);	\
	}while(0);										

#define mtos_wformat_weigth(buf, max_len, plen)				\
	mtos_fnone_for_webui(buf, max_len, plen)

#define mtos_wformat_search_id(buf, max_len, plen)			\
	mtos_fnone_for_webui(buf, max_len, plen)

#define mtos_wformat_vr(buf, max_len, plen)			\
	mtos_fzero_for_webui(buf, max_len, plen)

#define MINITOS_LIST_HEAD_NINIT		0
#define MINITOS_LIST_HEAD_INIT		1

enum {
    RT_ENODEV=118000, RT_ENOENTRY, RT_ESYS, RT_EMETRIC,     //118000-118003
    RT_ETYPE, RT_ESYNTAX, RT_EEXIST, RT_EDST,               //118004-118007
    RT_EGATEWAY, RT_ESRC, RT_ENETLINK, RT_EDEVTYPE,         //118008-118011
    RT_EFIBFULL, RT_EVRID, RT_EID, RT_EINVAL,               //118012-118015
    RT_EUNDEL,                                              //118016
    RT_PBR_NOMEM,              //118017
    RT_PBR_EEXIST,             //118018
    RT_PBR_NOEXSIT,            //118019
    RT_PBR_IP_EINVAL,          //118020
    RT_PBR_PORT_EINVAL,        //118021
    RT_PBR_PROTO_EINVAL,       //118022
    RT_PBR_GW_EINVAL,          //118023
    RT_PBR_DEV_EINVAL,         //118024
    RT_PBR_METRIC_EINVAL,      //118025
    RT_PBR_WEIGHT_EINVAL,      //118026
    RT_PBR_VR_EINVAL,          //118027
    RT_ESAMEAS_DIRECT,         //118028
    RT_PBR_IS_FULL,            //118029

    RT_EISPNAME,               //118030
    RT_EISPDATABASE,           //118031
    RT_EISPVERSION,            //118032
    RT_EISPMULTIPATH,          //118033
    RT_EISPCHECK,               //118034

    RT_PBR_EPROBEID,        //118035
    RT_PBR_LOADBALANCE,  //118036
    RT_EISP_TOO_MANY_NET,    //118037
    RT_PBR_LOADBALANCE_DIFF,    //118038
    RT_PBR_BANDWITH_DEV_ERR, //118039
    RT_ADDR_MASK_NOT_MATCH, //118040
    RT_MALLOC,			//118040
	RT_EISPNETWORK_UNREACHABLE = 118044
};

extern int get_route_by_inf(char *if_name, int ip_type, char *ip_str, char *mask, char **route_add_str);
extern void mtos_format_showbuf_for_netlink(struct minitos_routeshow *routeshow, size_t max_len, char *showbuf, size_t *buflen);

extern int minitos_format_filter_mroute_netlink(struct minitos_routeshow * routeshow,
		struct minitos_route *pmroute);
extern void mtos_format_oneroute_for_netlink(struct minitos_route *pmroute, unsigned int id,	\
		char *dst, char *gw, char *show_sflag, char *show_buf, size_t max_len, size_t *len);
extern int mtos_format1route_elemt_for_netlink_add(struct minitos_showlist *showlist, unsigned int id,	\
	char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len);
extern int mtos_format1route_elemt_for_netlink_del(struct minitos_showlist *showlist, unsigned int id,	\
	char *dst, char *gw, char *show_sflag, size_t buf_size, char *show_buf, size_t max_len, size_t *len);
extern int mtos_route_init_showlist(struct minitos_routeshow *routeshow);
extern int mtos_get_and_format_showbuf(struct minitos_routeshow *routeshow, int *buflen, char **buf);
extern void minitos_mslist_free(struct minitos_slist_head *mslist_head);
extern int minitos_iproute_execmd(char *buf, size_t buf_len);
extern void minitos_debug_showlist(struct minitos_routeshow *routeshow);

#define ROUTE_DEBUG(fmt, args...)	do{minitos_debug("[route][%s:%d]  "fmt"\n\n",__FUNCTION__,__LINE__, ##args);}while(0)
#endif
