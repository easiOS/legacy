#include <stdint.h>
#include <stddef.h>
#include <multiboot2.h>
#include <text.h>
#include <drivers/serial.h>

uint16_t __attribute__((aligned(4))) text_buffer[2400];
const char* cmdline = NULL;

void multiboot_enum(void* mbp);

void kmain(unsigned int magic, void* mbp)
{
	multiboot_enum(mbp);
}

void multiboot_enum(void* mbp)
{
	struct multiboot_tag* tag;
	unsigned int size;
	size = *(unsigned int*)mbp;
	if(size == 0)
		return;
	for(tag = (struct multiboot_tag*)(mbp + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag*)((unsigned short*)tag
			+ ((tag->size + 7) & ~7)))
	{
		switch(tag->type)
		{
			case MULTIBOOT_TAG_TYPE_CMDLINE:
			{
				struct multiboot_tag_string* tagstr =
					(struct multiboot_tag_string*)tag;
				cmdline = (tagstr->string);
				if(strcmp(cmdline, "serial") == 0)
				{
					sinit();
					if(sinitport(COM1) == -1)
					{
						printf("Failed to initialize serial port!\n");
					}
				}
				break;
			}
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{
				struct multiboot_tag_framebuffer_common* tagfb =
					(struct multiboot_tag_string*)tag;
				long long width = tagfb->framebuffer_width;
				long long height = tagfb->framebuffer_height;
				long long bpp = tagfb->framebuffer_bpp;
				long long pitch = tagfb->framebuffer_pitch;
				unsigned int addr = tagfb->framebuffer_addr;
				if(tagfb->framebuffer_type == 1)
				{
					vinit(width, height, bpp, pitch, addr);
					tinit(80, 30, (unsigned short*)text_buffer);
				}
				if(tagfb->framebuffer_type == 2)
				{
					tinit(width, height, (unsigned short*)addr);
				}
				break;
			}
			case MULTIBOOT_TAG_TYPE_MMAP:
			{
				struct multiboot_tag_mmap *tagmem =
					(struct multiboot_tag_mmap*)tag;
				int memn = (tagmem->size - 16) / sizeof(struct multiboot_mmap_entry);
				kminit(tagmem->entries, memn);
				break;
			}
			case MULTIBOOT_TAG_TYPE_MODULE:
			{
				printf("Module ignored\n");
				break;
			}
		}
	}
}