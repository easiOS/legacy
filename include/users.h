#ifndef H_USERS
#define H_USERS

#include <stdint.h>

typedef struct eos_user eos_user;

struct eos_user {
  uint8_t uid;
  uint32_t privileges;
  char un[64];
  char pw[128];
} __attribute__((packed));

struct eos_user_header {
  uint32_t magic;
  uint32_t users_n;
  struct eos_user users[0];
} __attribute__((packed));

void eos_users_init(struct eos_user_header* h);
int eos_users_auth(const char* username, const char* password);

#endif
