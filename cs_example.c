#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "deps/dragonnet/listen.h"
#include "dnet-types.h"

static DragonnetListener *l;

static void connect_func(DragonnetPeer *p)
{
	char *str_addr = dragonnet_addr_str(p->raddr);
	printf("new connection from %s\n", str_addr);
	free(str_addr);
}

static void handle_pingpacket(DragonnetPeer *p, PingPacket *ping)
{
	printf("PingPacket number: 0x%08x\n", ping->number);

	dragonnet_peer_send_PongPacket(p, &(PongPacket) {
		.number = ping->number
	});

	dragonnet_listener_close(l);
	dragonnet_listener_delete(l);
	l = NULL;

	dragonnet_peer_close(p);
	dragonnet_peer_delete(p);
}

static void handle_pongpacket(DragonnetPeer *p, PongPacket *pong)
{
	printf("PongPacket number: 0x%08x\n", pong->number);

	dragonnet_peer_close(p);
	dragonnet_peer_delete(p);
}

static void *srv_func(__attribute((unused)) void *unused)
{
	l = dragonnet_listener_new("[::1]:50000", &connect_func);
	assert(l != NULL);

	dragonnet_listener_set_recv_hook(l, DRAGONNET_TYPE_PINGPACKET,
			(void (*)(DragonnetPeer *, void *)) &handle_pingpacket);
	dragonnet_listener_run(l);
	return NULL;
}

static void *clt_func(__attribute((unused)) void *unused)
{
	while (l == NULL);

	DragonnetPeer *p = dragonnet_connect("[::1]:50000");
	assert(p != NULL);

	dragonnet_peer_set_recv_hook(p, DRAGONNET_TYPE_PONGPACKET,
			(void (*)(DragonnetPeer *, void *)) &handle_pongpacket);
	dragonnet_peer_run(p);

	dragonnet_peer_send_PingPacket(p, &(PingPacket) {
		.number = 0xdba
	});

	return NULL;
}

int main(__attribute((unused)) int argc, __attribute((unused)) char **argv)
{
	pthread_t srv_thread, clt_thread;
	pthread_create(&srv_thread, NULL, &srv_func, NULL);
	pthread_create(&clt_thread, NULL, &clt_func, NULL);

	pthread_join(clt_thread, NULL);
	pthread_join(srv_thread, NULL);

	while (true);
}
