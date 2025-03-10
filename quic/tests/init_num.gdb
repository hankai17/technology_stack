#statem/statem_dtls.c:773:        s->init_num = frag_len;
#statem/statem_dtls.c:794:        s->init_num = readbytes - 1;
#statem/statem_dtls.c:904:    *len = s->init_num = frag_len;
#statem/statem_dtls.c:1050:    frag = dtls1_hm_fragment_new(s->init_num, 0);
#statem/statem_dtls.c:1140:    s->init_num = frag->msg_header.msg_len + header_length;
#statem/statem_dtls.c:1280:    s->init_num = (int)msglen;

b ssl/statem/statem.c:858
b statem/statem_lib.c:105
b statem/statem_lib.c:1287
b statem/statem_lib.c:1337
b statem/statem_quic.c:60

#statem_ntls/ntls_statem_lib.c:84:    s->init_num = (int)msglen;
#statem_ntls/ntls_statem_lib.c:881:                s->init_num = readbytes - 1;
#statem_ntls/ntls_statem_lib.c:931:        s->init_num = SSL3_HM_HEADER_LENGTH;
#statem_ntls/ntls_statem_lib.c:957:        *len = (unsigned long)s->init_num;
#statem_ntls/ntls_statem_lib.c:962:    n = s->s3.tmp.message_size - s->init_num;
#statem_ntls/ntls_statem_lib.c:1020:    *len = s->init_num;
