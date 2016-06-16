#include <stdio.h>
#include <string.h>
#include <video.h>
#include <amethyst.h>
#include <dev/ethernet.h>
#include <net/routing.h>
#include "ifconfig.h"

void ifconfig_draw(am_win* w, int bx, int by)
{
	char buffer[256];
	int64_t px, py;
	struct ethernet_device* dev = (struct ethernet_device*)w->windata;
	snprintf(buffer, 256, "%s\nIPv4\n\t%d.%d.%d.%d / %d.%d.%d.%d\n", dev->name, dev->ipv4_address[0], dev->ipv4_address[1], dev->ipv4_address[2], dev->ipv4_address[3],
		dev->ipv4_netmask[0], dev->ipv4_netmask[1], dev->ipv4_netmask[2], dev->ipv4_netmask[3]);
	vsetcol(0, 0, 0, 255);
	vd_print(bx + 10, by + 10, buffer, &px, &py);
	snprintf(buffer, 256, "\tGateway: %d.%d.%d.%d\n\n\tReceived: %u\n\tSent: %u\n", dev->ipv4_gateway[0], dev->ipv4_gateway[1], dev->ipv4_gateway[2], dev->ipv4_gateway[3],
		dev->received, dev->sent);
	vd_print(px, py, buffer, &px, &py);
}

int ifconfig_main(int argc, char** argv)
{
	am_win* w = amethyst_create_window();
  if(!w) return 1;
  strcpy(w->title, "Network Interface Configuration");
  w->x = 100; w->y = 100;
  w->w = 720; w->h = 360;
 	w->bg.r = 212;
 	w->bg.g = 212;
 	w->bg.b = 212;
 	w->bg.a = 255;

 	const struct ethernet_device* dev = ethernet_getif(routing_table_getif(NULL, NULL));
 	if(!dev)
 	{
 		amethyst_destroy_window(w);
 		return 1;
 	}
 	w->windata = (void*)dev;
  w->draw = &ifconfig_draw;
  amethyst_set_active(w);
  return 0;
}