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

static void recv_type_func(DragonnetPeer *p, u16 type_id)
{
	char *str_addr = dragonnet_addr_str(p->raddr);
	printf("type %d from %s\n", type_id, str_addr);
	free(str_addr);

	if (type_id == DRAGONNET_TYPE_PINGPACKET) {
		PingPacket ping = dragonnet_peer_recv_PingPacket(p);
		printf("PingPacket number: 0x%08x\n", ping.number);

		PongPacket pong = {
			.number = ping.number
		};
		dragonnet_peer_send_PongPacket(p, pong);

		dragonnet_listener_close(l);
		dragonnet_listener_delete(l);
		l = NULL;

		dragonnet_peer_close(p);
		dragonnet_peer_delete(p);
	} else if (type_id == DRAGONNET_TYPE_PONGPACKET) {
		PongPacket pong = dragonnet_peer_recv_PongPacket(p);
		printf("PongPacket number: 0x%08x\n", pong.number);

		dragonnet_peer_close(p);
		dragonnet_peer_delete(p);
	}
}

static void *srv_func(__attribute((unused)) void *unused)
{
	l = dragonnet_listener_new("[::1]:50000", &connect_func, &recv_type_func);
	assert(l != NULL);

	dragonnet_listener_run(l);
	return NULL;
}

static void *clt_func(__attribute((unused)) void *unused)
{
	while (l == NULL);

	DragonnetPeer *p = dragonnet_connect("[::1]:50000", &recv_type_func);
	assert(p != NULL);

	dragonnet_peer_run(p);

	PingPacket ping = {
		.number = 0xdba
	};
	dragonnet_peer_send_PingPacket(p, ping);

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
