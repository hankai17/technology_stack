handle SIGPIPE noprint nopass nostop
set print pretty

b SSL_provide_quic_data
#b quic_change_cipher_state

b tls13_generate_handshake_secret
# 客户端构造的clienthello中的keyshare 没什么用
# server端是在构造serverhello时 gen hk secret的
# 客户端是在收到serverhello后才gen hk secret

#https://datatracker.ietf.org/doc/html/rfc8446#section-7.3

#xqc_tls_set_read_secret #收到serverhello包后 do_hk回调中 上层获取到 ssl库底层根据gen的hk secret产生的 r/w key
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

#b tls_process_finished
# 产生master_secret
# s->handshake_secret 产生 s->master_secret
