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

unsigned ip2int(const char* ip)
{
  unsigned v = 0;
  
  const char* start;
  start = ip;
  for(int i = 0; i < 4; i++)
  {
    char c;
    int n = 0;
    while(1)
    {
      c = *start;
      start++;
      if(c >= '0' && c <= '9')
      {
        n *= 10;
        n += c - '0';
      }
      else if((i < 3 && c == '.') || i == 3)
        break;
      else
        return 0;
    }
    if(n > 255)
      return 0;
    v *= 256;
    v += n;
  }
  return v;
}

int ifconfig_main(int argc, char** argv)
{
  struct ethernet_device* dev = (struct ethernet_device*)ethernet_getif(routing_table_getif(NULL, NULL));
  if(!dev)
    return 1;
  if(argc > 2)
  {
    uint8_t* octets = NULL;
    if(strcmp(argv[1], "setip") == 0)
      octets = dev->ipv4_address;
    if(strcmp(argv[1], "setmask") == 0)
      octets = dev->ipv4_netmask;
    if(strcmp(argv[1], "setgate") == 0)
      octets = dev->ipv4_gateway;
    if(octets)
    {
      unsigned ip = ip2int(argv[2]);
      if(ip != 0)
      {
        octets[0] = ip >> 24 & 0xff;
        octets[1] = ip >> 16 & 0xff;
        octets[2] = ip >> 8 & 0xff;
        octets[3] = ip & 0xff;
      }
      return 0;
    }
  }
	am_win* w = amethyst_create_window();
  if(!w) return 1;
  strcpy(w->title, "Network Interface Configuration");
  w->x = 100; w->y = 100;
  w->w = 720; w->h = 360;
 	w->bg.r = 212;
 	w->bg.g = 212;
 	w->bg.b = 212;
 	w->bg.a = 255;
 	w->windata = (void*)dev;
  w->draw = &ifconfig_draw;
  amethyst_set_active(w);
  return 0;
}