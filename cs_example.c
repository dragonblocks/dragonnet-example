#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "deps/dragonnet/listen.h"

static DragonnetListener *l;

static void connect_func(DragonnetPeer *p)
{
	char *str_addr = dragonnet_addr_str(p->raddr);
	printf("new connection from %s\n", str_addr);
	free(str_addr);

	dragonnet_listener_close(l);
	dragonnet_listener_delete(l);
	l = NULL;
}

static void *srv_func()
{
	l = dragonnet_listener_new("[::1]:50000", &connect_func);
	assert(l != NULL);

	dragonnet_listener_run(l);
	return NULL;
}

static void *clt_func()
{
	DragonnetPeer *p = dragonnet_connect("[::1]:50000");
	assert(p != NULL);

	dragonnet_peer_run(p);
	dragonnet_peer_close(p);
	dragonnet_peer_delete(p);
	return NULL;
}

int main()
{
	pthread_t srv_thread, clt_thread;
	pthread_create(&srv_thread, NULL, &srv_func, NULL);
	pthread_create(&clt_thread, NULL, &clt_func, NULL);

	pthread_join(clt_thread, NULL);
	pthread_join(srv_thread, NULL);
}
