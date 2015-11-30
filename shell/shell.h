#ifndef H_ESH
#define H_ESH
#include <stdint.h>
#include "../video.h"
#include "../keyboard.h"
#include "../kernel.h"
#include "../stdmem.h"
#include "../vfs.h"
#include "shell_cmds.h"
void shell_main();
typedef struct user {
	uint32_t id;
	char* name;
	bool superuser;
	char prompt;
	void* last; //hova kell exitelni
	char* passw;
} user_t;
user_t* shell_get_current_user();
void shell_request_exit();
bool shell_auth(char* uname);
#endif
