/* EasiOS luavm.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: Lua 5.1 bytecode interpreter
 */

#include <config.h>
#include <stdio.h>
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "luavm.h"
#ifdef __linux__ //we don't have these headers on easiOS
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#endif /* __linux__ */
#include <amethyst.h>

#ifdef LUAVM_DEBUG
#define luavm_inf(format, ...) printf("[luaVM INF] " format "\n", ##__VA_ARGS__)
#define luavm_war(format, ...) printf("[luaVM WAR] " format "\n", ##__VA_ARGS__)
#else /* LUAVM_DEBUG */
#define luavm_inf(format, ...)
#define luavm_war(format, ...)
#endif /* LUAVM_DEBUG */

#define luavm_err(format, ...) printf("[luaVM ERR] " format "\n", ##__VA_ARGS__)
#define luavm_fat(format, ...) printf("[luaVM FAT] " format "\n", ##__VA_ARGS__)

#define LVM_DEFINE_SYSCALL(name) void name (luavm_state* state, uint32_t reg);
LVM_DEFINE_SYSCALL(luavm_puts);
LVM_DEFINE_SYSCALL(luavm_tostring);
LVM_DEFINE_SYSCALL(luavm_setwindow_bg);
LVM_DEFINE_SYSCALL(luavm_sin);
LVM_DEFINE_SYSCALL(luavm_cos);
LVM_DEFINE_SYSCALL(luavm_video_plot);
LVM_DEFINE_SYSCALL(luavm_video_setcol);
LVM_DEFINE_SYSCALL(luavm_video_rect);

#define SYSCALLN 8

struct {
  const char* name;
  int id;
  void (*func)(luavm_state*, uint32_t);
} luavm_syscalls[SYSCALLN] = {
  {"puts", 0, &luavm_puts},
  {"tostring", 1, &luavm_tostring},
  {"setwin_bg", 2, &luavm_setwindow_bg},
  {"sin", 3, &luavm_sin},
  {"cos", 4, &luavm_cos},
  {"vplot", 5, &luavm_video_plot},
  {"vsetcol", 6, &luavm_video_setcol},
  {"vdrect", 7, &luavm_video_rect}
};

#ifdef __linux__
extern am_win w;

void* luacfile;

void luavm_spawn(lheader_t* f);
void luavm_loadfile(const char* fn);

static void signal_catch(int signo)
{
  printf("Signal caught: %s\n", strsignal(signo));
  if(signo == SIGINT || signo == SIGILL)
  {
    printf("exiting...\n");
    if(w.flags) if(w.unload) w.unload(&w);
    if(luacfile) free(luacfile);
    amethyst_destroy_window(&w);
    exit(0);
  }
}

int main(int argc, char** argv)
{
  printf("easiOS luaVM\n");
  if(argc < 2)
  {
    luavm_fat("Usage: %s filename", argv[0]);
    exit(4);
  }
  luavm_inf("setting up signal handler");
  if(signal(SIGINT, signal_catch) == SIG_ERR || signal(SIGILL, signal_catch) == SIG_ERR)
  {
    luavm_fat("error while setting the signal handler");
    return 3;
  }
  luavm_loadfile(argv[1]);
  luavm_spawn(luacfile);
  if(w.load) w.load(&w);
  while(w.flags && !(w.flags >> 2 & 1))
  {
    if(w.update) w.update(&w, 100);
  }
  return 0;
}

void luavm_loadfile(const char* fn)
{
  struct stat finfo;
  stat(fn, &finfo);
  luavm_inf("file size: %d bytes", finfo.st_size);
  FILE* f = fopen(fn, "rb");
  if(!f)
  {
    perror("Error opening file");
    exit(1);
  }
  luacfile = malloc(sizeof(char) * finfo.st_size);
  fread(luacfile, finfo.st_size, 1, f);
  fclose(f);
  lheader_t* header = (lheader_t*)luacfile;
  const char* sgn = "\33Lua";
  if(memcmp(header->signature, &sgn, 4) == 0)
  {
    luavm_fat("file not lua bytecode (0x%x)", header->signature);
    free(luacfile);
    exit(2);
  }
  if(header->ver != 0x51)
  {
    luavm_fat("file not lua 5.1 bytecode (%d)", header->ver);
    free(luacfile);
    exit(3);
  }
  luavm_inf("file ok");
}

#endif /* __linux__ */

void luavm_exec(luavm_state* state, uint32_t instruction);

#define LUAVM_OPCODE(instruction) instruction & 0x3F
#define LUAVM_A(instruction) instruction >> 6 & 0xFF
#define LUAVM_B(instruction) instruction >> 23 & 0x1FF
#define LUAVM_C(instruction) instruction >> 14 & 0x1FF
#define LUAVM_Bx(instruction) instruction >> 14
#define LUAVM_sBx(instruction) LUAVM_Bx(instruction)

void luavm_load()
{

}

void luavm_update(am_win* w, unsigned dt)
{
  if(!(w->flags >> 1 & 1)) return;
  luavm_state* s = (luavm_state*)w->windata;
  uint32_t* instr = (uint32_t*)((uint32_t)s->code_ptr);
  while(s->ip < s->code_n && ((w->flags >> 1) & 1))
  {
    luavm_exec(s, instr[s->ip]);
    s->ip++;
  }
  s->ip = 0;
} 

void luavm_draw(am_win* w, int bx, int by)
{

}

void luavm_spawn(lheader_t* f) //spawn a luavm and it's window
{
  #define CHECK(byte, shouldbe, errmsg) if(byte != shouldbe) {luavm_fat(errmsg "(%d)", byte); return;}
  const char* sgn = "\033Lua";
  if(memcmp(f->signature, &sgn, 4) == 0)
  {
    luavm_fat("not lua bytecode (sign: %.4s, should be: %.4s)", f->signature, sgn);
    return;
  }
  /*if(f->ver != 0x51)
  {
    printf("wrong version (should be 0x51)\n");
    return;
  }*/
  CHECK(f->ver, 0x51, "bad lua version");
  CHECK(f->format_ver, 0, "bad format");
  CHECK(f->endianness, 1, "bad endianness");
  CHECK(f->intsize, 4, "bad int size");
  CHECK(f->sizesize, 4, "bad size_t size");
  CHECK(f->instrsize, 4, "bad instruction size");
  CHECK(f->luansize, 8, "bad lua_Number size");
  CHECK(f->integral, 0, "bad integral flag");
  //printf("top func dump\n");
  void* topfunc = (void*)(((uint32_t)f) + 12);
  uint32_t srcnamel = *(uint32_t*)topfunc;
  //printf("srcname len: %d\n", srcnamel);
  //printf("srcname: \"");
  /*for(int i = 0; i < srcnamel; i++)
  {
    putc(*(char*)(topfunc + 4 + i), stdout);
    printf("%x ", *(char*)(topfunc + 4 + i));
  }
  printf("\"\n");*/

  //--------------------------------------
  am_win* w = amethyst_create_window();
  if(!w) return;
  strcpy(w->title, "LuaVM");
  w->x = 10;
  w->y = 10;
  w->w = 400;
  w->h = 300;
  w->bg.r = 212;
  w->bg.g = 212;
  w->bg.b = 212;
  w->bg.a = 255;
  w->windata = (void*)malloc(sizeof(luavm_state));
  if(!w->windata)
  {
    luavm_fat("cannot allocate memory for VM state");
    amethyst_destroy_window(w);
    return;
  }
  luavm_state* vm_state = (luavm_state*)w->windata;
  vm_state->window = w;
  vm_state->file = f;
  vm_state->ip = 0;
  vm_state->code_ptr = (uint32_t)(topfunc + 4 + srcnamel + 4 + 4 + 4 + 4);
  vm_state->code_n = *(uint32_t*)(vm_state->code_ptr - 4);
  vm_state->const_ptr = vm_state->code_ptr + vm_state->code_n * 4 + 4;
  vm_state->const_n = *(uint32_t*)(vm_state->const_ptr - 4);
  uint8_t* kptr = (uint8_t*)vm_state->const_ptr;
  for(int i = 0; i < vm_state->const_n; i++)
  {
    int len = 0;
    switch(*kptr)
    {
      case 0:

        luavm_inf("constant #%d: Nil", i);
        vm_state->constants[i].type = 0;
        kptr++;
        break;
      case 1:
        vm_state->constants[i].data = *(uint64_t*)(kptr+1);
        luavm_inf("constant #%d: %f (Boolean)", i, vm_state->constants[i].data);
        kptr += 9;
        break;
      case 3:
        //memcpy(&(vm_state->constants[i].data), kptr + 1, 8);
        vm_state->constants[i].data = *(uint64_t*)(kptr+1);
        luavm_inf("constant #%d: %f (Number)", i, vm_state->constants[i].data);
        kptr += 9;
        break;
      case 4:
        len = *(uint32_t*)(kptr + 1);
        vm_state->constants[i].data = (uint32_t)kptr + 5;
        luavm_inf("constant #%d: \"%s\" (String)", i, vm_state->constants[i].data);
        kptr += len + 5;
        for(int j = 0; j < SYSCALLN; j++)
        {
          if(strcmp((char*)(uint32_t)vm_state->constants[i].data, luavm_syscalls[j].name) == 0)
          {
            //printf("found sycall with name %s, assigning id %d\n", luavm_syscalls[j].name, luavm_syscalls[j].id);
            vm_state->globals[i].name = i;
            vm_state->globals[i].data = luavm_syscalls[j].id;
          }
        }
        break;
      default:
        luavm_err("unknown constant #%d with value 0x%x", i, vm_state->constants[i].data);
        break;
    }
  }
  vm_state->fprot_n = *(uint32_t*)(kptr);
  kptr += 4;
  if(vm_state->fprot_n != 0)
  {
    luavm_fat("bytecodes contains user-defined functions: this isn't allowed");
    free(w->windata);
    amethyst_destroy_window(w);
    return;
  }

  /*for(int i = 0; i < vm_state->fprot_n; i++)
  {
    uint32_t len = *(uint32_t*)kptr;
    kptr += 4 + len;
    uint32_t linedef = *(uint32_t*)kptr;
    kptr += 4;
    uint32_t lastlinedef = *(uint32_t*)kptr;
    printf("Func: Len: %d Linedef: %d Lastlinedef: %d\n", len, linedef, lastlinedef);
  }

  func_skip:*/
  //printf("Code n: %d\t@: 0x%x\nConst n: %d\t@: 0x%x\nFprot n: %d\t @:\n", vm_state->code_n, vm_state->code_ptr, vm_state->const_n, vm_state->const_ptr, vm_state->fprot_n);
  memset(vm_state->stack, 0, 200 * 4);
  w->flags |= 1 << 1;
  w->update = &luavm_update;
  w->draw = &luavm_draw;
  amethyst_set_active(w);
}

void luavm_exec(luavm_state* state, uint32_t instruction)
{
  uint32_t opcode = LUAVM_OPCODE(instruction);
  uint8_t a = LUAVM_A(instruction);
  uint16_t b = LUAVM_B(instruction);
  uint16_t c = LUAVM_C(instruction);
  int bx = LUAVM_Bx(instruction);
  double ra = 0.0, rb = 0.0, rc = 0.0;
  double* regs = (double*)state->registers;
  ra = regs[a];
  rb = regs[b];
  rc = regs[c];
  double /*ka,*/ kb, kc;
  //double* ad = (double*)&state->constants[a - 256].data;
  double* bd = (double*)&state->constants[b - 256].data;
  double* cd = (double*)&state->constants[c - 256].data;
  //if(a > 255) ka = *ad;
  if(b > 255) kb = *bd;
  if(c > 255) kc = *cd;
  double cb = (b > 255) ? kb : rb;
  double cc = (c > 255) ? kc : rc;
  switch(opcode)
  {
    case OP_MOVE:
      luavm_inf("move R(%u) := R(%u)", a, b);
      state->registers[a] = state->registers[b];
      break;
    case OP_LOADK: //load constant into register
      luavm_inf("load constant #%u into register %u", bx, a);
      memcpy(&state->registers[a], &state->constants[bx].data, 8);
      //state->registers[a] = state->constants[bx].data;
      //printf("\t\tconst val: %u %f reg val: %u %f\n", state->constants[bx].data, state->constants[bx].data, state->registers[a], state->registers[a]);
      break;
    case OP_LOADBOOL:
      luavm_inf("load constant boolean #%u into register %u (%s)", b, a, c ? "skip" : "no skip");
      regs[a] = b ? 1.0 : 0.0;
      if(c) state->ip++;
      break;
    case OP_SETGLOBAL: //set global value
      luavm_inf("set global #%u to value at register %u", bx, a);
      state->globals[bx].name = bx;
      //state->globals[bx].data = state->registers[a];
      memcpy(&state->globals[bx].data, &state->registers[a], 8);
      break;
    case OP_GETGLOBAL:
      luavm_inf("load global with name kst(%u) into register %u", bx, a);
      //printf("\t\t1. value at global: %f %u at register: %f %u\n", state->globals[bx].data, state->globals[bx].data, state->registers[a], state->registers[a]);
      //state->registers[a] = state->globals[bx].data;
      memcpy(&state->registers[a], &state->globals[bx].data, 8);
      //printf("\t\t2. value at global: %f %u at register: %f %u\n", state->globals[bx].data, state->globals[bx].data, state->registers[a], state->registers[a]);
      break;
    case OP_RETURN:
      luavm_inf("return");
      break;
    case OP_ADD:
      luavm_inf("R(%u)=RK(%u)+RK(%u)", a, b, c);
      regs[a] = cb + cc;
      //printf("\t\tresult: %f\n", state->registers[a]);
      break;
    case OP_SUB:
      luavm_inf("R(%u)=RK(%u)-RK(%u)", a, b, c);
      regs[a] = cb - cc;
      printf("\t\tresult: %f\n", state->registers[a]);
      break;
    case OP_MUL:
      luavm_inf("R(%u)=RK(%u)*RK(%u)", a, b, c);
      regs[a] = cb * cc;
      //printf("\t\tresult: %f\n", state->registers[a]);
      break;
    case OP_DIV:
      luavm_inf("R(%u)=RK(%u)/RK(%u)", a, b, c);
      regs[a] = cb / cc;
      //printf("\t\tresult: %f\n", state->registers[a]);
      break;
    case OP_MOD:
      luavm_inf("R(%u)=RK(%u)%%RK(%u)", a, b, c);
      regs[a] = cb - floor(cb/cc)*cc;
      //printf("\t\tresult: %f\n", state->registers[a]);
      break;
    case OP_POW:
      luavm_inf("R(%u)=R(%u)^R(%u)", a, b, c);
      //printf("\t\tR(b): %f R(c):%f\n", regs[b], regs[c]);
      regs[a] = pow(cb, cc);
      //printf("\t\tresult: %f\n", state->registers[a]);
      break;
    case OP_UNM:
      luavm_inf("R(%u)=-R(%u)", a, b);
      //printf("\t\tR(b): %f\n", regs[b]);
      regs[a] = -regs[b];
      //printf("\t\tresult: %f\n", state->registers[a]);
      break;
    case OP_LT:
      luavm_inf("RK(%u) < RK(%u) != %u", b, c, a);
      if((cb < cc) != a)
        state->ip++;
      break;
    case OP_LE:
      luavm_inf("RK(%u) <= RK(%u) != %u", b, c, a);
      if((cb <= cc) != a)
        state->ip++;
      break;
    case OP_EQ:
      luavm_inf("RK(%u) == RK(%u) != A", b, c);
      if((cb == cc) != a) state->ip++;
      break;
    case OP_TEST:
      luavm_inf("R(%u) == %u", a, c);
      if((ra != 0.0) == c)
      {
        state->ip++;
      }
      break;
    case OP_TESTSET:
      luavm_inf("R(%u) == %u SET", b, c);
      if((rb != 0.0) == c)
      {
        state->ip++;
      }
      else
      {
        regs[a] = rb;
      }
      break;
    case OP_LOADNIL:
      luavm_inf("LOADNIL %u->%u", a, b);
      for(int i = a; i < b; i++)
      {
        state->registers[i] = 0;
      }
      break;
    case OP_JMP:
      #ifdef __linux__
      luavm_inf("JMP %+d", bx - 131071);
      #else
      luavm_inf("JMP %d", bx - 131071);
      #endif /* __linux__ */
      state->ip +=  bx - 131071;
      break;
    case OP_CALL:
      luavm_inf("call function at register %d (%u)", a, state->registers[a]);
      //if(b == 0) printf("\t\twith an unknown number of parameters\n");
      //if(b == 1) printf("\t\twithout parameters\n");
      //if(b > 1) printf("\t\twith a number of %d parameters\n", b - 1);
      for(int i = 0; i < SYSCALLN; i++)
      {
        if(luavm_syscalls[i].id == state->registers[a])
        {
          luavm_syscalls[i].func(state, a);
          break;
        }
      }
      break;
    default:
      luavm_fat("unimplemented opcode %d", opcode);
      #ifdef __linux__
      raise(SIGILL);
      #else
      amethyst_destroy_window(state->window);
      #endif /* __linux__ */
      break;
  }
}

void luavm_puts(luavm_state* state, uint32_t reg)
{
  char* s = (char*)(uint32_t)state->registers[reg + 1];
  puts(s);
}

void luavm_tostring(luavm_state* state, uint32_t reg)
{
  //TODO: this shit leakin memory, fix asap fam
  double* args = (double*)&state->registers[reg];
  uint64_t* argsi64 = (uint64_t*)args;
  char* buf = (char*)malloc(64);
  snprintf(buf, 64, "%d", (int)args[1]);
  argsi64[0] = (uint32_t)buf;
}

void luavm_setwindow_bg(luavm_state* state, uint32_t reg)
{
  double* args = (double*)&state->registers[reg];
  double r = args[1];
  double g = args[2];
  double b = args[3];
  double a = args[4];
  state->window->bg.r = (int)r;
  state->window->bg.g = (int)g;
  state->window->bg.b = (int)b;
  state->window->bg.a = (int)a;
}

void luavm_sin(luavm_state* state, uint32_t reg)
{
  double* args = (double*)&state->registers[reg];
  args[0] = sin(args[1]);
}

void luavm_cos(luavm_state* state, uint32_t reg)
{
  double* args = (double*)&state->registers[reg];
  args[0] = cos(args[1]);
}

void luavm_video_plot(luavm_state* state, uint32_t reg)
{
  int wx = state->window->x, wy = state->window->y;
  double* args = (double*)&state->registers[reg];
  vplot(wx + (int)args[1], wy + (int)args[2]);
}

void luavm_video_setcol(luavm_state* state, uint32_t reg)
{
  double* args = (double*)&state->registers[reg];
  vsetcol((int)args[1], (int)args[2], (int)args[3], (int)args[4]);
}

void luavm_video_rect(luavm_state* state, uint32_t reg)
{
  int wx = state->window->x, wy = state->window->y;
  double* args = (double*)&state->registers[reg];
  int drawmode = (int)args[1];
  int x = (int)args[2];
  int y = (int)args[3];
  int w = (int)args[4];
  int h = (int)args[5];
  vd_rectangle(drawmode, wx + x, wy + y, w, h);
}