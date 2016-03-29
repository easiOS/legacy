#ifndef H_IOEVENTS
#define H_IOEVENTS

typedef enum {
	USERINPUT,
} ioe_t;

struct input_event {
	unsigned flags;
	/* flags:
	 * Bits from right to left:
	 * # if 1 this is a keyboard event
	 * # reserved
	 * # reserved
	 * # if 1 this was a release
	 * # if 1, shift was held
	 * # if 1, ctrl was held
	 * # if 1, alt was held
	 * # if 1, gui was held
	 * # if 1, there were multiple scans
	 */
	unsigned keycode;
	char character;
};

struct ioevent {
	ioe_t event_type;
	union {
		struct input_event userinput;
	} event;
};

void ioe_push(struct ioevent* e);
struct ioevent* ioe_pop();
int ioe_available(void);
void ioeinit(void);

#endif