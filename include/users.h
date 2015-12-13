#ifndef H_USERS
#define H_USERS

#include <stdint.h>

typedef struct eos_user eos_user;

struct eos_user {
  char un[64];
  char pw_md5[128];
  char fullname[64];
  int building, room;
  char phone[32];
  char other[64];
} __attribute__((packed));

struct eos_user_header {
  uint32_t magic;
  uint32_t users_n;
  struct eos_user users[0];
} __attribute__((packed));

void eos_users_init(struct eos_user_header* h);

#endif
