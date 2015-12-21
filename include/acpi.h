#ifndef H_ACPI
#define H_ACPI

#include <stdint.h>

struct acpi_sdt_header {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__((packed));

struct rsdp_desc {
  char signature[8];
  uint8_t checksum;
  char oemid[6];
  uint8_t revision;
  struct acpi_sdt_header* rsdt;
} __attribute__((packed));

struct rsdp_desc2 {
  struct rsdp_desc desc1;
  uint32_t length;
  uint64_t xsdt_addr;
  uint8_t extended_checksum;
  uint8_t reserved[3];
} __attribute__((packed));



struct rsdp_desc* acpi_findrsdp();
struct rsdp_desc* acpi_getrsdp();

#endif
