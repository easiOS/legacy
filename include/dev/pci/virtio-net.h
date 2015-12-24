#ifndef H_VIRTIONET
#define H_VIRTIONET

#include <stdint.h>

struct virtio_vqueue
{
  struct
  {
    uint64_t address; // 64-bit address of the buffer on the guest machine.
    uint32_t length;   // 32-bit length of the buffer.
    uint16_t flags;  // 1:Next field contains linked buffer index.  2:Buffer is write-only (clear for read-only).  4:Buffer contains additional buffer addresses.
    uint16_t next;   // If flag is set, contains index of next buffer in chain.
  } buffers[2];
  struct
  {
    uint16_t flags;            // 1: Do not trigger uint32_terrupts.
    uint16_t index;            // Index of the next ring index to be used.  (Last available ring buffer index+1)
    uint16_t ring[2];  // List of available buffer indexes from Buffers array above.
    uint16_t interrupt;   // If enabled, device will trigger uint32_terrupt after this ring index has been processed.
  } available;
  uint8_t padding[4096];  // Reserved
  // 4096 byte alignment
  struct
  {
    uint16_t flags;            // 1: Do not notify device when buffers are added to available ring.
    uint16_t index;            // Index of the next ring index to be used.  (Last used ring buffer index+1)
    struct
    {
      uint32_t index;  // Index of the used buffer in the Buffers array above.
      uint32_t length; // Total bytes written to buffer.
    } ring[2];
    uint16_t interrupt;   // If enabled, device will trigger interrupt after this ring index has been used.
  } used;
} __attribute__((packed));

struct PacketHeader
{
  uint8_t Flags;            // 1: Needs checksum
  uint8_t Segmentation;     // 0:None 1:TCPv4 3:UDP 4:TCPv6 0x80:ECN
  uint16_t HeaderLength;    // Size of header to be used during segmentation.
  uint16_t SegmentLength;   // Maximum segment size (not including header).
  uint16_t ChecksumStart;   // The position to begin calculating the checksum.
  uint16_t ChecksumOffset;  // The position after ChecksumStart to store the checksum.
  uint16_t BufferCount;     // Used when merging buffers.
} __attribute__((packed));

void virtionetinit(uint8_t bus, uint8_t slot);

#endif
