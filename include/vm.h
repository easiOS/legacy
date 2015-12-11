#ifndef H_EP_VM
#define H_EP_VM

//Eelphant Virtual Machine

struct eelphant_vm {
  uint32_t ax, bx, cx, dx, sp, ip, flags;
  uint8_t* memory;
};

#endif
