#include <eelphant.h>
#include "testapp.h"
#include <string.h>
#include <dev/serial.h>
#include <dev/ethernet.h>
#include <net/slip.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <stdio.h>

void testapp_load(ep_window* w)
{
	/*eth_dev_t* dev = slip_dev_init(COM1);
	if(!dev)
	{
		return;
	}
	w->userdata[0] = (uint32_t)dev;*/
	ethernet_list();
	/*uint8_t buffer[128];
	strcpy(buffer + 8, "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
	udp_create((udp_header*)buffer, 35261, 80, buffer + 8, 124);
	printf("create udp\n");
	slip_send_packet(buffer, 128, dev);
	printf("sent packet!\n");*/
	//ping test
  	uint8_t localip[4] = {127, 0, 0, 1};
  	icmp_send_ping_req(localip, localip);
}

void testapp_update(uint64_t dt, ep_window*w)
{
}

void testapp_init(void)
{
	ep_window* w = eelphant_create_window();
	if(!w)
	{
		printf("testapp: can't allocate window\n");
		return;
	}
	w->w = 640;
	w->h = 480;
	w->x = 100;
	w->y = 100;
	w->load = &testapp_load;
	w->update = &testapp_update;
	strcpy(w->title, "Testapp");
	w->load(w);
	eelphant_switch_active(w);
}