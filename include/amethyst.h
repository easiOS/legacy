#ifndef H_AMETHYST
#define H_AMETHYST

#include <video.h>

#define AM_MAX_WINDOWS 32

typedef struct am_win am_win;

typedef enum {
	KEYPRESS, KEYRELEASE, TICK
} am_event_type;

typedef struct {
	am_event_type type;
	unsigned flags;
	union {
		struct {
			int keycode;
			char character;
			// flags: ... u c s a d
		} key;
		struct {
			unsigned time;
		} tick;
	} data;
} am_event;

#define AM_KEYEVENT_ISCTRL(e) (((e)->flags >> 3) & 1) 
#define AM_KEYEVENT_ISSHIFT(e) (((e)->flags >> 2) & 1)
#define AM_KEYEVENT_ISALT(e) (((e)->flags >> 1) & 1)
#define AM_KEYEVENT_ISDUBS(e) ((e)->flags & 1)

#define AM_KEYEVENT_SETCTRL(e) ((e)->flags |= (1 >> 3)) 
#define AM_KEYEVENT_SETSHIFT(e) ((e)->flags |= (1 >> 2))
#define AM_KEYEVENT_SETALT(e) ((e)->flags |= (1 >> 1))
#define AM_KEYEVENT_SETDUBS(e) ((e)->flags |= 1)

#define AM_KEYEVENT_GETKEYCODE(e) ((e)->data.key.keycode)
#define AM_KEYEVENT_GETCHAR(e) ((e)->data.key.character)

struct am_win {
	int x, y; // coordinates of window
	int w, h; // dimensions of window
	int z; // level of window
	char title[64]; // title of window
    unsigned flags; //flags
    //flags:
    ///31-1 0
    ///rrTKF
    ///F - focused
    ///K - request key event
    ///T - request tick event
    int (*load)(am_win*); // loads the program itself; args: (PTR to window); returns: status code, (0 = ok)
    int (*unload)(am_win*); // called before the window is deconstructed; args: (PTR to window); returns: status code, (0 = ok)
    void (*update)(am_win*, unsigned); // called every frame; args: (PTR to window, milliseconds elapsed since last call); returns: nothing
    void (*draw)(am_win*, int, int); // called every frame; args: (PTR to window, X coordinate of top-left point of window,
    								 // Y coordinate of top-left point of window); returns: nothing
    void (*event)(am_win*, am_event*); // called every event; args: (PTR to window, PTR to event); returns: nothing
    void* windata; // PTR to memory where the window is able to store it's data
    rgb_t bg; // background color of window
};

#define AM_WIN_FOCUSED(w) (((w)->flags >> 1) & 1)
#define AM_WIN_KEYEV(w) (((w)->flags >> 1) & 1)
#define AM_WIN_TICKEV(w) (((w)->flags >> 2) & 1)
#define AM_WIN_SETKEYEV(w) ((w)->flags |= (1 << 1))
#define AM_WIN_SETTICKEV(w) ((w)->flags |= (1 << 2))

typedef struct {
	char name[64]; // args[0]
	int argc; // always atleast 1
	int (*main)(int argc, char** argv);
} am_cmd_t; // Amethyst command definition

// PTR to window, ...

am_win* amethyst_create_window();
void amethyst_destroy_window(am_win* w);
int amethyst_main(int width, int height); // to be called by the kernel_main
int amethyst_init(am_win* wmw, int x, int y, int width, int height); // initializes the window manager
void amethyst_set_active(am_win* w);

#endif /* H_AMETHYST */