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
    puts("  Password hash: ");
    char b[32];
    for(int j = 0; j < 4; j++)
    {
      int num = ((uint32_t*)(users[i].pw_md5))[j];
      puts(itoa(num, b, 16));
    }
    putc('\n');
    puts("\n--------------------\n");
  }
  users_n = h->users_n;
}
