#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#include "../include/users.h"

int main(int argc, char** argv)
{
  struct eos_user_header header;
  header.magic = 0x0237C0C0;
  int n = 0;
  char inp[128];
  eos_user* users = NULL;
  printf("EasiOS v0.3.x Userfile Generator\n");
  while(1)
  {
    eos_user user;
    printf("Add new user? (y/n) ");
    scanf("%s", &inp);
    if(inp[0] == 'n') break;
    printf("UID: ");
    scanf("%d", &user.uid);
    printf("Username (max 64 chars): ");
    scanf("%s", &inp);
    memcpy(user.un, inp, 64); memset(inp, 0, 64);
    unsigned char digest[16];
    MD5_CTX context;
    MD5_Init(&context);
    printf("Password: ");
    char password[128];
    scanf("%s", &password);
    MD5_Update(&context, &password[0], strlen(password));
    MD5_Final((unsigned char*)&user.pw_md5, &context);
    users = realloc(users, sizeof(eos_user)* (n + 1));
    memcpy(&users[n], &user, sizeof(eos_user));
    n++;
    memset(inp, 0, 64);
  }
  puts("outta loop\n");
  header.users_n = (n > 0 ? n : 0);
  FILE* f = fopen("out.euf", "w");
  puts("opened\n");
  fwrite(&header, sizeof(struct eos_user_header), 1, f);
  puts("written header\n");
  if(users)
  {
    fseek(f, -sizeof(struct eos_user), SEEK_CUR);
    fwrite(users, sizeof(eos_user), n + 1, f);
    free(users);
    puts("written users\n");
  }
  puts("closing\n");
  fclose(f);
  printf("Done!\n");
}
