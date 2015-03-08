#ifndef H_ESH
#define H_ESH
#include <stdint.h>
#include "../video.h"
#include "../keyboard.h"
#include "../kernel.h"
#include "shell_cmds.h"
void shell_main();
typedef struct user {
	uint32_t id;
	char* name;
	char* passw;
	char prompt;
	bool superuser;
	void* last; //hova kell exitelni
} user_t;
user_t* shell_get_current_user();
void shell_request_exit();
#endif
