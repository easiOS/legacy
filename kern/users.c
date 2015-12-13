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
  char b[64];
  puts("Users:\n--------------------\n");
  for(int i = 0; i < h->users_n; i++)
  {
    puts("  Username: "); puts(users[i].un);
    puts("\n  Password hash: "); puts(users[i].pw_md5);
    puts("\n  Full name: "); puts(users[i].fullname);
    puts("\n  Building: "); puts(itoa(users[i].building, b, 10));
    puts("\n  Room: "); puts(itoa(users[i].room, b, 10));
    puts("\n  Phone: "); puts(users[i].phone);
    puts("\n  Other information: "); puts(users[i].other);
    puts("\n--------------------\n");
  }
  users_n = h->users_n;
}
