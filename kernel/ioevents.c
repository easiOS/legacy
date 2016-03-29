#include <ioevents.h>
#include <stdlib.h>
#include <string.h>

struct ioevent ioevents[256];
int ioev_in = 0, ioev_out = 0;

struct ioevent* ioe_pop(struct ioevent* in)
{
	if(ioev_in == ioev_out)
		return NULL; //queue is empty
	struct ioevent* e = &ioevents[ioev_out];
	memcpy(in, e, sizeof(struct ioevent));
	ioev_out = (ioev_out + 1) % 257;
	return e;
}

void ioe_push(struct ioevent* e)
{
	if(ioev_in == ((ioev_out - 258) % 257))
		return; //queue is full
	struct ioevent* es = &ioevents[ioev_in];
	memcpy(es, e, sizeof(struct ioevent));
	ioev_in = (ioev_in + 1) % 257;
}

int ioe_available(void)
{
	return ioev_in != ioev_out;
}