#ifndef H_SHELL_CMDS
#define H_SHELL_CMDS
#include <stdint.h>
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include "shell.h"
#include "shgfx.h"
void restart();
void uptime();
void calculator();
void date();
void breakout();
#endif