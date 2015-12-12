#ifndef H_EP_VM
#define H_EP_VM

enum OpMode {iABC, iABx, iAsBx, iAx};

struct luac_header {
  uint32_t magic; //must be 0x1B4C7561
  uint8_t version; //must be 0x51 (Lua 5.1)
  uint8_t format; //must be 0
  uint8_t endianness;
  uint8_t sizeof_int;
  uint8_t sizeof_sizet;
  uint8_t sizeof_instruction;
  uint8_t sizeof_lua_n;
  uint8_t integral;
};

//Eelphant Virtual Machine

struct eelphant_vm {
  uint32_t ra, rb, rc, ax, bx, pc, k, upval, gbl;
  uint8_t* memory;
};

#endif
