handle SIGPIPE noprint nopass nostop
set print pretty

#xqc_tls_set_read_secret
b xqc_tls.c:1177 if level == ssl_encryption_handshake
commands 
p secret
p *secret
p secret_len
c
end

#xqc_tls_set_write_secret
b xqc_tls.c:1219 if level == ssl_encryption_handshake
commands 
p secret
p *secret
p secret_len
c
end


b xqc_conn_tls_crypto_data_cb
#b quic_get_message

#hk首包 108字节 构造的是ServerHello的第二部分extension 这个extension是hk的首包 (跟init的serverhello里的extension不同的是这个包)需要用hk的密钥加密发送
#b tls_construct_extensions   
#b ssl/statem/extensions.c:875 
#commands
#p *thisexd
#c
#end

#hk第二包 804字节 添加证书链
b ssl_add_cert_to_wpacket

#hk第三个包  
b tls_construct_cert_verify

#hk第四个包
b tls_construct_finished
