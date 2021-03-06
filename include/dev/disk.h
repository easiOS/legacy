#ifndef H_DISK
#define H_DISK

#include <stdint.h>

struct chs_addr {
	uint8_t head, sector, cylinder;
} __attribute__((packed));

struct mbr_pe {
	uint8_t status;
	struct chs_addr begin;
	uint8_t type;
	struct chs_addr end;
	uint32_t lba;
	uint32_t sectors;
} __attribute__((packed));

struct mbr {
	uint8_t code[446];
	struct mbr_pe partitions[4];
	uint16_t signature; // 0xAA55 in memory, 0x55AA on disk
}__attribute__((packed));

struct guid {
	uint32_t data1;
	uint16_t data2, data3;
	uint32_t data4, data5;
} __attribute__((packed));

struct gpt_hdr {
	uint64_t signature; //gpt signature (0x5452415020494645 little-endian)
	uint32_t revision; //gpt revision
	uint32_t hdrsiz; //header size
	uint32_t crc32; //crc32 of header
	uint32_t zero; //must be zero
	uint64_t curlba; //current lba
	uint64_t bcklba; //header backup lba
	uint64_t ftulba; //first usable lba
	uint64_t ltulba; //last usable lba
	struct guid disk_guid; //disk GUID
	uint64_t pealba; //partition entry array lba
	uint32_t nope; //number of partition entries
	uint32_t pesiz; //partition entry size
	uint32_t peacrc; //partition entry array crc32
	uint8_t reserved[420]; //must be all zero
} __attribute__((packed));

struct gpt_pe {
	struct guid ptype; 	//EasiOS 0.3 Partition GUID:
	                   	//6054BBB2-E732-4645-85CA-B58CC586C7D7
	struct guid pguid; //partition guid
	uint64_t startlba; //start of partition
	uint64_t lastlba; //end of partition (inclusive)
	uint64_t flags; //flags
	uint8_t pname[72]; //partition name UTF-16LE
} __attribute__((packed));

struct eos_drives { //physical or virtual partition using the EOS initrd filesystem 
	char letter;
	char type; //0 = physical, 1 = virtual, 2 = FAT32 Physical
	union {
		struct {
			uint32_t lba;
			uint32_t size; //sectors
		} phys;
		uint32_t virt;
	} address;
};

//ATA STATUS

#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Inlex
#define ATA_SR_ERR     0x01    // Error

//ATA ERROR CODES

#define ATA_ER_BBK      0x80    // Bad sector
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // No media
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // No media
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

//ATA COMMANDS

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC
#define ATAPI_CMD_READ       0xA8
#define ATAPI_CMD_EJECT      0x1B

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
 
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

// Channels:
#define      ATA_PRIMARY      0x00
#define      ATA_SECONDARY    0x01
 
// Directions:
#define      ATA_READ      0x00
#define      ATA_WRITE     0x01

void disk_init();
void ide_init(int bus, int slot);
int ide_read_sector(int LBA, void* ide_buf, int count, int slavebit);
int ide_write_sector(int LBA, void* ide_buf, int count, int slavebit);

#endif