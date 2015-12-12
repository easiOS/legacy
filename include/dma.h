#ifndef H_DMA
#define H_DMA

#include <stdint.h>

struct dma_block {
  uint8_t page;
  uint32_t offset, length;
};

void dma_pageoffset(struct dma_block* blk, uint8_t* data);

#endif
