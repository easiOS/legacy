#ifndef H_LUAVM
#define H_LUAVM

#include <amethyst.h>

typedef struct luabc_header {
  char signature[4]; //0x1B4C7561
  uint8_t ver; //0x51
  uint8_t format_ver; //0
  uint8_t endianness; //1 (lil)
  uint8_t intsize; //4
  uint8_t sizesize; //4
  uint8_t instrsize; //4
  uint8_t luansize; //8
  uint8_t integral; //0
} lheader_t;

typedef struct luavm_const{
  uint8_t type;
  uint64_t data; //contains value, unless type equals 0, then it's nil, or 4, then it's a pointer to string
} luavm_const;

typedef struct luavm_state {
  am_win* window;
  lheader_t* file;
  uint32_t stack[200];
  uint32_t ip;
  uint32_t code_n, code_ptr, const_n, const_ptr, fprot_n, fprot_ptr;
  luavm_const constants[200];
  uint64_t registers[200];
  struct {
    uint32_t name;
    uint64_t data;
  } globals[200];
} luavm_state;

typedef struct luavm_func {
  void* ptrblock;

} luavm_func;

enum {
OP_MOVE,/*  A B R(A) := R(B)          */
OP_LOADK,/* A Bx  R(A) := Kst(Bx)         */
OP_LOADBOOL,/*  A B C R(A) := (Bool)B; if (C) pc++      */
OP_LOADNIL,/* A B R(A) := ... := R(B) := nil      */
OP_GETUPVAL,/*  A B R(A) := UpValue[B]        */

OP_GETGLOBAL,/* A Bx  R(A) := Gbl[Kst(Bx)]        */
OP_GETTABLE,/*  A B C R(A) := R(B)[RK(C)]       */

OP_SETGLOBAL,/* A Bx  Gbl[Kst(Bx)] := R(A)        */
OP_SETUPVAL,/*  A B UpValue[B] := R(A)        */
OP_SETTABLE,/*  A B C R(A)[RK(B)] := RK(C)        */

OP_NEWTABLE,/*  A B C R(A) := {} (size = B,C)       */

OP_SELF,/*  A B C R(A+1) := R(B); R(A) := R(B)[RK(C)]   */

OP_ADD,/* A B C R(A) := RK(B) + RK(C)       */
OP_SUB,/* A B C R(A) := RK(B) - RK(C)       */
OP_MUL,/* A B C R(A) := RK(B) * RK(C)       */
OP_DIV,/* A B C R(A) := RK(B) / RK(C)       */
OP_MOD,/* A B C R(A) := RK(B) % RK(C)       */
OP_POW,/* A B C R(A) := RK(B) ^ RK(C)       */
OP_UNM,/* A B R(A) := -R(B)         */
OP_NOT,/* A B R(A) := not R(B)        */
OP_LEN,/* A B R(A) := length of R(B)        */

OP_CONCAT,/*  A B C R(A) := R(B).. ... ..R(C)     */

OP_JMP,/* sBx pc+=sBx         */

OP_EQ,/*  A B C if ((RK(B) == RK(C)) ~= A) then pc++    */
OP_LT,/*  A B C if ((RK(B) <  RK(C)) ~= A) then pc++      */
OP_LE,/*  A B C if ((RK(B) <= RK(C)) ~= A) then pc++      */

OP_TEST,/*  A C if not (R(A) <=> C) then pc++     */
OP_TESTSET,/* A B C if (R(B) <=> C) then R(A) := R(B) else pc++ */

OP_CALL,/*  A B C R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1)) */
OP_TAILCALL,/*  A B C return R(A)(R(A+1), ... ,R(A+B-1))    */
OP_RETURN,/*  A B return R(A), ... ,R(A+B-2)  (see note)  */

OP_FORLOOP,/* A sBx R(A)+=R(A+2);
      if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) }*/
OP_FORPREP,/* A sBx R(A)-=R(A+2); pc+=sBx       */

OP_TFORLOOP,/*  A C R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));
                        if R(A+3) ~= nil then R(A+2)=R(A+3) else pc++ */
OP_SETLIST,/* A B C R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B  */

OP_CLOSE,/* A   close all variables in the stack up to (>=) R(A)*/
OP_CLOSURE,/* A Bx  R(A) := closure(KPROTO[Bx], R(A), ... ,R(A+n))  */

OP_VARARG/* A B R(A), R(A+1), ..., R(A+B-1) = vararg    */
};

void luavm_spawn(lheader_t* f);

#endif
