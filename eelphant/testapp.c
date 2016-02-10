#include <eelphant.h>
#include "testapp.h"
#include <string.h>
#include <dev/serial.h>
#include <dev/ethernet.h>
#include <net/slip.h>
#include <net/udp.h>

void testapp_load(ep_window* w)
{
	eth_dev_t* dev = slip_dev_init(COM1);
	w->userdata[0] = (uint32_t)dev;
}

void testapp_init(void)
{
	ep_window* w = eelphant_create_window();
	if(!w)
	{
		printf("testapp: can't allocate window\n");
		return;
	}
	w->load = &testapp_load;
	strcpy(w->title, "Testapp");
	w->load(w);
	eelphant_switch_active(w);
}