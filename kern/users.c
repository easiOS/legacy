#include <users.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

eos_user* users;
int users_n = 0;

void eos_users_init(struct eos_user_header* h)
{
  if(h->magic != 0x0237C0C0) return;
  if(users) free(users);
  if(h->users_n < 1) return;
  users = (eos_user*)malloc((sizeof(eos_user)) * h->users_n);
  memcpy(users, h->users, sizeof(eos_user) * h->users_n);
  puts("Users:\n--------------------\n");
  for(int i = 0; i < h->users_n; i++)
  {
    puts("  Username: "); puts(users[i].un);
    puts("\n  Password: "); puts(users[i].pw);
    puts("\n--------------------\n");
  }
  users_n = h->users_n;
}

int eos_users_auth(const char* username, const char* password)
{
  int ret = 1; //0 - OK, 1 - User not found, 2 - Bad Password
  for(int i = 0; i < users_n; i++)
  {
    if(strcmp(username, users[i].un) == 0)
    {
      if(strcmp(password, users[i].pw) == 0)
      {
        ret = 0;
        break;
      }
      ret = 2;
      break;
    }
  }
  return ret;
}
