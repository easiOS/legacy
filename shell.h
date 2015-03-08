#ifndef H_ESH
#define H_ESH
#include <stdint.h>
#include "video.h"
#include "keyboard.h"
void shell_main();
typedef struct user {
	uint32_t id;
	char* name;
	char* passw;
	char prompt;
	bool superuser;
	void* last; //hova kell exitelni
} user_t;
#endif
