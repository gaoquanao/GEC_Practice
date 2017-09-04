#include "websocket.h"
#include "socket.h"

static noPollConn * conn_tmp;

void listener_on_message (noPollCtx * ctx, noPollConn * conn, noPollMsg * msg, noPollPtr  user_data) {
		// print the message (for debugging purposes) and reply
		// 如果msg不是八个字节, 就return
        if(nopoll_msg_get_payload_size (msg) != 8){
        	printf("msg len illegal,size = %d\n",nopoll_msg_get_payload_size(msg));
        	return;
        }
        unsigned int * msg_tmp = (unsigned int *)nopoll_msg_get_payload(msg);

        printf ("Listener received (size: %d, ctx refs: %d): fragment: %d '0x%8x ,0x%8x'\n", 
                nopoll_msg_get_payload_size (msg),
                nopoll_ctx_ref_count (ctx), nopoll_msg_is_fragment (msg), *msg_tmp,*(msg_tmp + 1));

        send_data((char *)msg_tmp, nopoll_msg_get_payload_size(msg)); //转发从微信端传回的数据
        printf("[listener_on_message]%s\n", (char *)msg_tmp);
  
        return;
}

void __nopoll_regression_on_close (noPollCtx * ctx, noPollConn * conn, noPollPtr user_data)
{
	printf ("Reg test: called connection close (TLS: %d)..\n", nopoll_conn_is_tls_on (conn));
	// connection_close_count++;
	conn_tmp = NULL;
	return;
}

nopoll_bool on_connection_opened (noPollCtx * ctx, noPollConn * conn, noPollPtr user_data)
{
	/* set connection close */
	nopoll_conn_set_on_close (conn, __nopoll_regression_on_close, NULL);
	conn_tmp = conn;
	/* notify connection accepted */
	printf ("INFO: connection received from %s, with Host: %s and Origin: %s\n",
	   nopoll_conn_host (conn), nopoll_conn_get_host_header (conn), nopoll_conn_get_origin (conn));
	return nopoll_true;
}

int init_websocket_v12()
{
	conn_tmp = NULL;
	//创建nopoll的上下文
	noPollCtx      * ctx = NULL;
	ctx = nopoll_ctx_new ();

	noPollConnOpts * opts;
	opts = nopoll_conn_opts_new();
	// with TLS (TLSv1.2)
	nopoll_conn_opts_set_ssl_protocol(opts, NOPOLL_METHOD_TLSV1_2);

	//监听
	noPollConn *listener = nopoll_listener_tls_new_opts(ctx,opts,"0.0.0.0","12345");
	if(!nopoll_conn_is_ok(listener)){
		printf("listerner error!\n");
		return -1;
	}

	if (! nopoll_ctx_set_certificate (ctx, NULL, "test-certificate.crt", "test-private.key", NULL)) {
		printf ("ERROR: unable to setup certificates at context level..\n");
		return -1;
	}
	//设置消息处理函数位listerner_on_message
	nopoll_ctx_set_on_msg(ctx,listener_on_message,NULL);

	//设置open处理函数
	nopoll_ctx_set_on_open (ctx, on_connection_opened, NULL);
	//等待事件通知
	printf("websocket start wait connect\n");
	nopoll_loop_wait(ctx,0);
	return 0;

}

int websocket_send(void * msg,unsigned int len)
{
	if(conn_tmp == NULL)
	{
		printf("websocket not ready!\n");
		return -1;
	}
	unsigned int * msg_tmp = msg;
	printf("[websocket_send] : 0x%8x, 0x%8x\n", *msg_tmp, *(msg_tmp+1));
	nopoll_conn_send_binary(conn_tmp, msg, len);
}