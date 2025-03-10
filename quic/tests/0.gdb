handle SIGPIPE noprint nopass nostop
set print pretty

#b SSL_do_handshake

#b SSL_write
#b SSL_read
#b sendto
#b recvfrom
#b xqc_packet_decrypt

#b xqc_packet_encrypt_buf
#b xqc_tls_encrypt_payload
#b xqc_gen_long_packet_header
#b xqc_gen_short_packet_header
#b xqc_process_packet_with_pn
#b xqc_ossl_aead_encrypt
#b xqc_conn.c:1986
#b xqc_create_hs_buffer
#b xqc_gen_crypto_frame
#b xqc_conn_schedule_packets
##b xqc_crypto_stream_send
#b xqc_stream.c:1094
#b xqc_conn_schedule_packets_to_paths

#b xqc_conn_tls_crypto_data_cb
#b xqc_enc_packet_with_pn
#b xqc_crypto_stream_send
#b xqc_conn_schedule_packets
#b xqc_packet_out.c:291 if ret == 39
#b xqc_stream.c:1071
#b xqc_enc_packet_with_pn


#b xqc_crypto_create
#b xqc_packet_decrypt_single
#b xqc_tls_decrypt_header
#b xqc_crypto_derive_packet_protection_iv
#b xqc_ossl_aead_decrypt

#b xqc_tls.c:462
#b xqc_tls_encrypt_payload

#b SSL_provide_quic_data
#b xqc_conn_tls_crypto_data_cb
#b xqc_create_crypto_stream
#b xqc_process_crypto_write_streams
#b xqc_path_send_packets
#b xqc_enc_packet_with_pn

#b xqc_crypto_create
#b xqc_conn_tls_crypto_data_cb
#b xqc_tls_set_read_secret
#b xqc_tls_encrypt_payload
#b xqc_crypto_encrypt_payload


#b xqc_packet_decrypt
#b xqc_tls_add_handshake_data
b SSL_provide_quic_data
#
b xqc_tls_set_read_secret
#b xqc_tls_set_write_secret
#b xqc_tls_add_handshake_data
#b xqc_tls_flush_flight

#b xqc_crypto_derive_keys

b xqc_tls.c:1177 if level == ssl_encryption_handshake
commands 
p secret
p *secret
p secret_len
c
end

#b xqc_crypto_create
#b xqc_packet_decrypt
#b xqc_conn_tls_crypto_data_cb
#b xqc_tls.c:1177 if level == ssl_encryption_handshake

#解密payload
#b xqc_crypto.c:360 
#commands
#p ckm->key.base
#p nonce
#c
#end

#加密payload
#b xqc_crypto.c:319
#commands
#p ckm->key.base
#p nonce
#c
#end

