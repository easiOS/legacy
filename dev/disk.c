#include <dev/disk.h>
#include <dev/pci.h>
#include <dev/timer.h>
#include <port.h>
#include <stdio.h>
#include <vfs.h>
#include <fs/thinfat32.h>
#include <string.h>

struct IDEChannelRegisters {
   unsigned short base;  // I/O Base.
   unsigned short ctrl;  // Control Base
   unsigned short bmide; // Bus Master IDE
   unsigned char  nIEN;  // nIEN (No Interrupt);
} channels[2];

struct ide_device {
   unsigned char  Reserved;    // 0 (Empty) or 1 (This Drive really exists).
   unsigned char  Channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
   unsigned char  Drive;       // 0 (Master Drive) or 1 (Slave Drive).
   unsigned short Type;        // 0: ATA, 1:ATAPI.
   unsigned short Signature;   // Drive Signature
   unsigned short Capabilities;// Features.
   unsigned int   CommandSets; // Command Sets Supported.
   unsigned int   Size;        // Size in Sectors.
   unsigned char  Model[41];   // Model in string.
} ide_devices[4];

extern TFInfo tf_info;
extern TFFile tf_file;

struct eos_drives drives[4] = {0};

unsigned char ide_buf[2048] = {0};
unsigned static char ide_irq_invoked = 0;
uint32_t package[2];

unsigned char ide_read(unsigned char channel, unsigned char reg);
void ide_write(unsigned char channel, unsigned char reg, unsigned char data);
void ide_read_buffer(unsigned char channel, unsigned char reg, unsigned int buffer,
                     unsigned int quads);
unsigned char ide_polling(unsigned char channel, unsigned int advanced_check);
unsigned char ide_print_error(unsigned int drive, unsigned char err);
void ide_read_sectors(unsigned char drive, unsigned char numsects, unsigned int lba,
                      //unsigned short es, unsigned int edi);
						unsigned int out);
int ide_get_first_hdd();
void ide_detect_partitions();

void disk_init()
{
	memset(drives, 0, sizeof(struct eos_drives) * 4);
}

void ide_init(int bus, int slot)
{
	printf("Initializing IDE on bus %x slot %x\n", bus, slot);
	for(int i = 0; i < 4; i++)
	{
		if(ide_devices[i].Reserved)
		{
			printf("IDE controller already initialized\n");
			return;
		}
	}
	int i, j, k, count = 0;
	//Get BAR values
	uint32_t bar0, bar1, bar2, bar3, bar4;
	bar0 = pci_config_read_dword(bus, slot, 0, 0x10);
  	bar1 = pci_config_read_dword(bus, slot, 0, 0x14);
  	bar2 = pci_config_read_dword(bus, slot, 0, 0x18);
  	bar3 = pci_config_read_dword(bus, slot, 0, 0x1C);
  	bar4 = pci_config_read_dword(bus, slot, 0, 0x20);
  	//Set I/O ports
	channels[ATA_PRIMARY  ].base  = (bar0 & 0xFFFFFFFC) + 0x1F0 * (!bar0);
   	channels[ATA_PRIMARY  ].ctrl  = (bar1 & 0xFFFFFFFC) + 0x3F6 * (!bar1);
   	channels[ATA_SECONDARY].base  = (bar2 & 0xFFFFFFFC) + 0x170 * (!bar2);
   	channels[ATA_SECONDARY].ctrl  = (bar3 & 0xFFFFFFFC) + 0x376 * (!bar3);
   	channels[ATA_PRIMARY  ].bmide = (bar4 & 0xFFFFFFFC) + 0; // Bus Master IDE
   	channels[ATA_SECONDARY].bmide = (bar4 & 0xFFFFFFFC) + 8; // Bus Master IDE
   	//Disable IRQs
   	//ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
   	//ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);
   	ide_write(ATA_PRIMARY, ATA_REG_CONTROL, channels[ATA_PRIMARY].nIEN = (ide_irq_invoked = 0x0) + 0x02);
   	ide_write(ATA_PRIMARY, ATA_REG_CONTROL, channels[ATA_SECONDARY].nIEN = (ide_irq_invoked = 0x0) + 0x02);
   	//Detect ATA/ATAPI devices
   	for (i = 0; i < 2; i++)
      for (j = 0; j < 2; j++) {
         unsigned char err = 0, type = IDE_ATA, status;
         ide_devices[count].Reserved = 0; // Assuming that no drive here.
 
         // (I) Select Drive:
         ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
         sleep(1); // Wait 1ms for drive select to work.
 
         // (II) Send ATA Identify Command:
         ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
         sleep(1); // This function should be implemented in your OS. which waits for 1 ms.
                   // it is based on System Timer Device Driver.
         // (III) Polling:
         if (ide_read(i, ATA_REG_STATUS) == 0) continue; // If Status = 0, No Device.
         while(1) {
            status = ide_read(i, ATA_REG_STATUS);
            if ((status & ATA_SR_ERR)) {err = 1; break;} // If Err, Device is not ATA.
            if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break; // Everything is right.
         }
 
         // (IV) Probe for ATAPI Devices:
 
         if (err != 0) {
            unsigned char cl = ide_read(i, ATA_REG_LBA1);
            unsigned char ch = ide_read(i, ATA_REG_LBA2);
 
            if (cl == 0x14 && ch ==0xEB)
               type = IDE_ATAPI;
            else if (cl == 0x69 && ch == 0x96)
               type = IDE_ATAPI;
            else
               continue; // Unknown Type (may not be a device).
 
            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
            sleep(1);
         }
         // (V) Read Identification Space of the Device:
         ide_read_buffer(i, ATA_REG_DATA, (unsigned int) ide_buf, 128);
         // (VI) Read Device Parameters:
         unsigned short* signature = (unsigned short*)(ide_buf + ATA_IDENT_DEVICETYPE), *capabilities = (unsigned short*)(ide_buf + ATA_IDENT_CAPABILITIES);
         unsigned int* cmdsets = (unsigned int*)(ide_buf + ATA_IDENT_COMMANDSETS);
         ide_devices[count].Reserved     = 1;
         ide_devices[count].Type         = type;
         ide_devices[count].Channel      = i;
         ide_devices[count].Drive        = j;
         ide_devices[count].Signature    = *signature;
         ide_devices[count].Capabilities = *capabilities;
         ide_devices[count].CommandSets  = *cmdsets;
 
         // (VII) Get Size:
         if (ide_devices[count].CommandSets & (1 << 26))
            // Device uses 48-Bit Addressing:
            ide_devices[count].Size   = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
         else
            // Device uses CHS or 28-bit Addressing:
            ide_devices[count].Size   = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA));
 
         // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
         for(k = 0; k < 40; k += 2) {
            ide_devices[count].Model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
            ide_devices[count].Model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];}
         ide_devices[count].Model[40] = 0; // Terminate String.
         count++;
      }
 
   // 4- Print Summary:
   for (i = 0; i < 4; i++)
      if (ide_devices[i].Reserved == 1) {
         printf(" Found %s Drive %dGB - %s\n",
            (const char *[]){"ATA", "ATAPI"}[ide_devices[i].Type],         /* Type */
            ide_devices[i].Size / 1024 / 1024 / 2,               /* Size */
            ide_devices[i].Model);
      }
    ide_detect_partitions();
}

unsigned char ide_read(unsigned char channel, unsigned char reg) {
   unsigned char result = 0;
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   if (reg < 0x08)
      result = inb(channels[channel].base + reg - 0x00);
   else if (reg < 0x0C)
      result = inb(channels[channel].base  + reg - 0x06);
   else if (reg < 0x0E)
      result = inb(channels[channel].ctrl  + reg - 0x0A);
   else if (reg < 0x16)
      result = inb(channels[channel].bmide + reg - 0x0E);
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
   return result;
}

void ide_write(unsigned char channel, unsigned char reg, unsigned char data) {
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   if (reg < 0x08)
      outb(channels[channel].base  + reg - 0x00, data);
   else if (reg < 0x0C)
      outb(channels[channel].base  + reg - 0x06, data);
   else if (reg < 0x0E)
      outb(channels[channel].ctrl  + reg - 0x0A, data);
   else if (reg < 0x16)
      outb(channels[channel].bmide + reg - 0x0E, data);
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

void ide_read_buffer(unsigned char channel, unsigned char reg, unsigned int buffer,
                     unsigned int quads) {
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
   asm("pushw %es; pushw %ax; movw %ds, %ax; movw %ax, %es; popw %ax;"); 
   if (reg < 0x08)
      insl(channels[channel].base  + reg - 0x00, (void*)buffer, quads);
   else if (reg < 0x0C)
      insl(channels[channel].base  + reg - 0x06, (void*)buffer, quads);
   else if (reg < 0x0E)
      insl(channels[channel].ctrl  + reg - 0x0A, (void*)buffer, quads);
   else if (reg < 0x16)
      insl(channels[channel].bmide + reg - 0x0E, (void*)buffer, quads);
   asm("popw %es;");
   if (reg > 0x07 && reg < 0x0C)
      ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

unsigned char ide_polling(unsigned char channel, unsigned int advanced_check) {
 
   // (I) Delay 400 nanosecond for BSY to be set:
   // -------------------------------------------------
   for(int i = 0; i < 4; i++)
      ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns; loop four times.
 
   // (II) Wait for BSY to be cleared:
   // -------------------------------------------------
   while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
      ; // Wait for BSY to be zero.
 
   if (advanced_check) {
      unsigned char state = ide_read(channel, ATA_REG_STATUS); // Read Status Register.
 
      // (III) Check For Errors:
      // -------------------------------------------------
      if (state & ATA_SR_ERR)
         return 2; // Error.
 
      // (IV) Check If Device fault:
      // -------------------------------------------------
      if (state & ATA_SR_DF)
         return 1; // Device Fault.
 
      // (V) Check DRQ:
      // -------------------------------------------------
      // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
      if ((state & ATA_SR_DRQ) == 0)
         return 3; // DRQ should be set
 
   }
 
   return 0; // No Error.
 
}

unsigned char ide_print_error(unsigned int drive, unsigned char err) {
   if (err == 0)
      return err;
 
   puts("IDE:");
   if (err == 1) {puts("- Device Fault\n     "); err = 19;}
   else if (err == 2) {
      unsigned char st = ide_read(ide_devices[drive].Channel, ATA_REG_ERROR);
      if (st & ATA_ER_AMNF)   {puts("- No Address Mark Found\n     ");   err = 7;}
      if (st & ATA_ER_TK0NF)   {puts("- No Media or Media Error\n     ");   err = 3;}
      if (st & ATA_ER_ABRT)   {puts("- Command Aborted\n     ");      err = 20;}
      if (st & ATA_ER_MCR)   {puts("- No Media or Media Error\n     ");   err = 3;}
      if (st & ATA_ER_IDNF)   {puts("- ID mark not Found\n     ");      err = 21;}
      if (st & ATA_ER_MC)   {puts("- No Media or Media Error\n     ");   err = 3;}
      if (st & ATA_ER_UNC)   {puts("- Uncorrectable Data Error\n     ");   err = 22;}
      if (st & ATA_ER_BBK)   {puts("- Bad Sectors\n     ");       err = 13;}
   } else  if (err == 3)           {puts("- Reads Nothing\n     "); err = 23;}
     else  if (err == 4)  {puts("- Write Protected\n     "); err = 8;}
   printf("- [%s %s] %s\n",
      (const char *[]){"Primary", "Secondary"}[ide_devices[drive].Channel], // Use the channel as an index into the array
      (const char *[]){"Master", "Slave"}[ide_devices[drive].Drive], // Same as above, using the drive
      ide_devices[drive].Model);
 
   return err;
}

unsigned char ide_ata_access(unsigned char direction, unsigned char drive, unsigned int lba, 
                             unsigned char numsects, unsigned int out) {
	unsigned char lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */, dma /* 0: No DMA, 1: DMA */, cmd;
 	unsigned char lba_io[6];
   	unsigned int  channel      = ide_devices[drive].Channel; // Read the Channel.
   	unsigned int  slavebit      = ide_devices[drive].Drive; // Read the Drive [Master/Slave]
   	unsigned int  bus = channels[channel].base; // Bus Base, like 0x1F0 which is also data port.
   	unsigned int  words      = 256; // Almost every ATA drive has a sector-size of 512-byte.
   	unsigned short cyl, i;
   	unsigned char head, sect, err;

   	ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);
   	   // (I) Select one from LBA28, LBA48 or CHS;
   if (lba >= 0x10000000) { // Sure Drive should support LBA in this case, or you are
                            // giving a wrong LBA.
      // LBA48:
      lba_mode  = 2;
      lba_io[0] = (lba & 0x000000FF) >> 0;
      lba_io[1] = (lba & 0x0000FF00) >> 8;
      lba_io[2] = (lba & 0x00FF0000) >> 16;
      lba_io[3] = (lba & 0xFF000000) >> 24;
      lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
      lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
      head      = 0; // Lower 4-bits of HDDEVSEL are not used here.
   } else if (ide_devices[drive].Capabilities & 0x200)  { // Drive supports LBA?
      // LBA28:
      lba_mode  = 1;
      lba_io[0] = (lba & 0x00000FF) >> 0;
      lba_io[1] = (lba & 0x000FF00) >> 8;
      lba_io[2] = (lba & 0x0FF0000) >> 16;
      lba_io[3] = 0; // These Registers are not used here.
      lba_io[4] = 0; // These Registers are not used here.
      lba_io[5] = 0; // These Registers are not used here.
      head      = (lba & 0xF000000) >> 24;
   } else {
      // CHS:
      lba_mode  = 0;
      sect      = (lba % 63) + 1;
      cyl       = (lba + 1  - sect) / (16 * 63);
      lba_io[0] = sect;
      lba_io[1] = (cyl >> 0) & 0xFF;
      lba_io[2] = (cyl >> 8) & 0xFF;
      lba_io[3] = 0;
      lba_io[4] = 0;
      lba_io[5] = 0;
      head      = (lba + 1  - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
   }
   dma = 0;
   while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY); // Wait if busy.
   if (lba_mode == 0)
      ide_write(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
   else
      ide_write(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA
  if (lba_mode == 2) {
      ide_write(channel, ATA_REG_SECCOUNT1,   0);
      ide_write(channel, ATA_REG_LBA3,   lba_io[3]);
      ide_write(channel, ATA_REG_LBA4,   lba_io[4]);
      ide_write(channel, ATA_REG_LBA5,   lba_io[5]);
   }
   ide_write(channel, ATA_REG_SECCOUNT0,   numsects);
   ide_write(channel, ATA_REG_LBA0,   lba_io[0]);
   ide_write(channel, ATA_REG_LBA1,   lba_io[1]);
   ide_write(channel, ATA_REG_LBA2,   lba_io[2]);
   if (lba_mode == 0 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
   if (lba_mode == 1 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;   
   if (lba_mode == 2 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO_EXT;   
   if (lba_mode == 0 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
   if (lba_mode == 1 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
   if (lba_mode == 2 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA_EXT;
   if (lba_mode == 0 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
   if (lba_mode == 1 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
   if (lba_mode == 2 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO_EXT;
   if (lba_mode == 0 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
   if (lba_mode == 1 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
   if (lba_mode == 2 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA_EXT;
   ide_write(channel, ATA_REG_COMMAND, cmd);               // Send the Command.
   if (dma)
      if (direction == 0);
         // DMA Read.
      else;
         // DMA Write.
   else
      if (direction == 0)
         // PIO Read.
      for (i = 0; i < numsects; i++) {
         if ((err = ide_polling(channel, 1)))
            return err; // Polling, set error and exit if there is.
        printf("reading sector %d\n", i);
        insw(bus, (void*)out, words);
        out += (words * 2);
        /*
         asm("pushw %es");
         asm("mov %%ax, %%es" : : "a"(selector));
         asm("rep insw" : : "c"(words), "d"(bus), "D"(edi)); // Receive Data.
         asm("popw %es");
         edi += (words*2);*/
      } else {
      // PIO Write.
         for (i = 0; i < numsects; i++) {
            ide_polling(channel, 0); // Polling.
            //asm("pushw %ds");
            //asm("mov %%ax, %%ds"::"a"(selector));
            //asm("rep outsw"::"c"(words), "d"(bus), "S"(edi)); // Send Data
            //asm("popw %ds");
            //edi += (words*2);
         }
         ide_write(channel, ATA_REG_COMMAND, (char []) {   ATA_CMD_CACHE_FLUSH,
                        ATA_CMD_CACHE_FLUSH,
                        ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
         ide_polling(channel, 0); // Polling.
      }
 
   return 0; // Easy, isn't it?
}

void ide_read_sectors(unsigned char drive, unsigned char numsects, unsigned int lba,
                      unsigned int out) {
 
   // 1: Check if the drive presents:
   // ==================================
   if (drive > 3 || ide_devices[drive].Reserved == 0) package[0] = 0x1;      // Drive Not Found!
 
   // 2: Check if inputs are valid:
   // ==================================
   else if (((lba + numsects) > ide_devices[drive].Size) && (ide_devices[drive].Type == IDE_ATA))
      package[0] = 0x2;                     // Seeking to invalid position.
 
   // 3: Read in PIO Mode through Polling & IRQs:
   // ============================================
   else {
      unsigned char err = 0;
      if (ide_devices[drive].Type == IDE_ATA)
         //err = ide_ata_access(ATA_READ, drive, lba, numsects, es, edi);
      	err = ide_ata_access(ATA_READ, drive, lba, numsects, out);
      /*else if (ide_devices[drive].Type == IDE_ATAPI)
         for (i = 0; i < numsects; i++)
            err = ide_atapi_read(drive, lba + i, 1, es, edi + (i*2048));*/
      package[0] = ide_print_error(drive, err);
   }
}

int ide_get_first_hdd()
{
	for(int i = 0; i < 4; i++)
	{
		if(ide_devices[i].Reserved)
		{
			if(ide_devices[i].Type == IDE_ATA)
			{
				return i;
			}
		}
	}
	return -1;
}

int ide_read_sector(int LBA, void* ide_buf, int count, int slavebit) {
   int stat;

   outb(0x3f6, 0x02);   //disable interrupts

   outb(0x1F6, (0xE0 | (slavebit <<  4) | (LBA >> 24 & 0x0F)));

   //waste some time
   outb(0x1f1, 0x00);

   //set the sector count
   outb(0x1f2, (unsigned char)count);
   sleep(1);
   stat = ide_polling(0, 0);
      if(stat != 0)
         return stat;
   //send the low 8 bits of lbs to 1f3
   outb(0x1f3, (unsigned char)LBA);
   sleep(1);
   stat = ide_polling(0, 0);
      if(stat != 0)
         return stat;
   //send the middle 8 bits to 1f4
   outb(0x1f4, (unsigned char)(LBA >> 8));
   sleep(1);
   stat = ide_polling(0, 0);
      if(stat != 0)
         return stat;
   //send the high 8 to 1f5
   outb(0x1f5, (unsigned char)(LBA >> 16));
   sleep(1);
   stat = ide_polling(0, 0);
   if(stat != 0)
      return stat;
   //issue a read sectors command
   outb(0x1f7, 0x20);
   sleep(1);
   stat = ide_polling(0, 0);
      if(stat != 0)
         return stat;

   //eat 256 words form the buffer
   insw(0x1f0, ide_buf, 256);
   
   ide_polling(0,0);
   return 0;
}

int ide_write_sector(int LBA, void* outarray, int count, int slavebit)  {
   int stat=0;

   outb(0x3f6, 0x02);   //disable interrupts

   outb(0x1F6, (0xE0 | (slavebit <<  4) | (LBA >> 24 & 0x0F)));

   //waste some time
   outb(0x1f1, 0x00);

   //set the sector count
   outb(0x1f2, (unsigned char)count);
   sleep(1);
   stat = ide_polling(0, 1);
      if(stat == 1 || stat ==2)
         printf("error after 1f2  ");
   //send the low 8 bits of lbs to 1f3
   outb(0x1f3, (unsigned char)LBA);
   sleep(1);
   stat = ide_polling(0,1);
      if(stat == 1 || stat ==2)
         printf("error after 1f3  ");
   //send the middle 8 bits to 1f4
   outb(0x1f4, (unsigned char)(LBA >> 8));
   sleep(1);
   stat = ide_polling(0, 1);
      if(stat == 1 || stat ==2)
         printf("error after 1f4  ");
   //send the high 8 to 1f5
   outb(0x1f5, (unsigned char)(LBA >> 16));
   sleep(1);
   stat = ide_polling(0, 1);
   if(stat == 1 || stat ==2)
         printf("error after 1f5  ");
   //issue a WRITE sectors command
   outb(0x1f7, 0x30);
   sleep(1);
   stat = ide_polling(0, 1);
      if(stat == 1 || stat ==2)
         printf("error after write command  ");

   //output 256 words form the buffer
   
   outsw(0x1f0,outarray, 256);

   stat = ide_polling(0, 1);
      if(stat == 1 || stat ==2)
         printf("error after write data  ");

   outb(0x1f7, 0xE7); //flush the cache after each write command

   ide_polling(0,1);
   return 0;
}

void ide_detect_gpt_partitions()
{
   printf("Searching GPT partitions\n");
   uint8_t gptbuffer[512];
   struct gpt_hdr* gpt = (struct gpt_hdr*)gptbuffer;
   ide_read_sector(1, gpt, 1, 0);
   if(gpt->signature != 0x5452415020494645)
   {
      printf("Invalid GPT signature 0x%x\n", gpt->signature);
      return;
   }
   printf("GPT Version 0x%x\n", gpt->revision);
   for(int i = 0; i < 32; i++)
   {
      struct gpt_pe partitions[4];
      ide_read_sector(gpt->pealba + i, partitions, 1, 0);
      for(int p = 0; p < 4; p++)
      {
         uint64_t zero[2]; memset(zero, 0, 16);
         if(memcmp(zero, &partitions[p].ptype, 16) == 0)
            continue; //skip unused entry
         struct guid eos_ptype = {0x6054bbb2, 0xe732, 0x4645, 0x8cb5ca85, 0xd7c786c5};
         if(memcmp(&partitions[p].ptype, &eos_ptype, 16) == 0)
         {
            printf("easiOS 0.3 System Partition found!\n");
            int di = -1;
            for(int j = 0; j < 4; j++)
            {
               if(drives[j].letter == 0)
               {
                  di = j;
                  break;
               }
            }
            if(di == -1)
            {
               printf("  Cannot add more drives\n");
               return;
            }
            tf_info.driveid = di;
            drives[di].letter = 'a' + di;
            drives[di].type = 2;
            drives[di].address.phys.lba = partitions[i].startlba;
            drives[di].address.phys.size = partitions[i].lastlba + 1 - partitions[i].startlba;
            tf_init();
            tf_info.driveid = di;
            tf_mkdir((uint8_t*)"/sys", 0); tf_mkdir((uint8_t*)"/bin", 0);
            tf_mkdir((uint8_t*)"/cfg", 0); tf_mkdir((uint8_t*)"/user", 0);
            return;
         }
      }
   }
}

void ide_detect_partitions()
{
   printf("Searching partitions\n");
	uint8_t mbrbuffer[512];
	uint8_t partbuffer[512];
   //Check for GPT
   struct gpt_hdr* gpt = (struct gpt_hdr*)mbrbuffer;
   ide_read_sector(1, gpt, 1, 0);
   if(gpt->signature == 0x5452415020494645)
   {
      printf("GPT found\n");
      ide_detect_gpt_partitions();
      return;
   }
   //Check for MBR
	struct mbr* mbr = (struct mbr*)mbrbuffer;
    ide_read_sector(0, mbr, 1, 0);
    if(mbr->signature != 0xAA55)
    {
    	printf("No MBR found\n");
    	return;
    }
   	for(int i = 0; i < 4; i++)
   	{
         printf("Partition type: 0x%x, lba: 0x%x, size: 0x%x\n", mbr->partitions[i].type, mbr->partitions[i].lba, mbr->partitions[i].sectors);
   		if(mbr->partitions[i].type == 0xee)
         {
            printf("Protective MBR found, assuming GPT partition table\n");
            ide_detect_gpt_partitions();
            return;
         }
         if(mbr->partitions[i].type == 0xb)
   		{
   			if(tf_info.type == 1)
   			{
   				continue;
   			}
   			printf("Partition found %d at LBA %d (size: %d) FAT32\n", i, mbr->partitions[i].lba, mbr->partitions[i].sectors);
   			tf_info.type = 1;
   			int di = -1;
   			for(int j = 0;j < 4; j++)
   			{
   				if(drives[j].letter == 0)
   				{
   					di = j;
   					break;
   				}
   			}
   			if(di == -1)
   			{
   				printf("  Cannot add more drives\n");
   				return;
   			}
   			tf_info.driveid = di;
   			drives[di].letter = 'a' + di;
   			drives[di].type = 2;
   			drives[di].address.phys.lba = mbr->partitions[i].lba;
   			drives[di].address.phys.size = mbr->partitions[i].sectors;
   			tf_init();
   			tf_info.driveid = di;
   		}
   		if(mbr->partitions[i].type == 0x7f)
   		{
   			printf("Partition found %d at LBA %d (size: %d)\n", i, mbr->partitions[i].lba, mbr->partitions[i].sectors);
   			ide_read_sector(mbr->partitions[i].lba, partbuffer, 1, 0);
   			struct initramfs_header* vfs = (struct initramfs_header*)partbuffer;
   			if(vfs->magic != 0x45524653)
   			{
   				printf("  Not EOS partition\n");
   			}
   			int di = -1;
   			for(int j = 0;j < 4; j++)
   			{
   				if(drives[j].letter == 0)
   				{
   					di = j;
   					break;
   				}
   			}
   			if(di == -1)
   			{
   				printf("  Cannot add more drives\n");
   				return;
   			}
   			drives[di].letter = 'a' + di;
   			drives[di].type = 0;
   			drives[di].address.phys.lba = mbr->partitions[i].lba;
   			drives[di].address.phys.size = mbr->partitions[i].sectors;
   		}
   	}
}