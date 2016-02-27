#ifndef H_IOWIN
#define H_IOWIN

#define IOWIN_MODE_OPEN 0
#define IOWIN_MODE_SAVE 1

void iowin_spawn(int mode, char* name, void* data, size_t* len, ep_window* parent);

#endif