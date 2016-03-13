#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <eelphant.h>
#include <video.h>
#include <vfs.h>
#include <dev/disk.h>
#include <fs/thinfat32.h>
#include <fs/thinternal.h>
#include "notepad.h"

#define EFM_SELECTED_FILE 0
#define EFM_DIRENTRIES 1
#define EFM_DIRENTRIESC 2

typedef struct EFM_FileEntry {
	char filename[64];    //fed cba9 876 543 210
	uint16_t permissions; //uuu uuuu rwx rwx rwx TBI
	uint16_t owner, group; //user & group id TBI
	uint8_t attributes; //Bit 0: read-only. Bit 1: hidden. Bit 2: sysfile. Bit 3: vol label. Bit 4: subdir. Bit 5: archive.
	uint16_t cTime, cDate; //FAT32 datetime format (5/6/5 bits, for hour/minutes/doubleseconds)
	uint16_t mTime, mDate; //FAT32 datetime format (7/4/5 bits, for year-since-1980/month/day)
	uint32_t fileSize; //in bytes
} EFM_FileEntry;

extern struct eos_drives drives[4];

//Convert MSDOS 8.3 filename to null-terminated string
//buf should be at least 13 bytes long (filename + dot + extension + '\0')
char* efm_msdosfn2str(char* buf, uint8_t* fn, uint8_t* ext)
{
	if(!buf || !fn)
		return NULL;
	memset(buf, 0, 13);
	char filename[9];
	char extension[4];
	memcpy(filename, fn, 8); filename[8] = 0;
	memcpy(extension, ext, 3); extension[3] = 0;
	for(int i = 7; i > 0; i--)
		if(filename[i] == ' ')
			filename[i] = 0;

	for(int i = 3; i > 0; i--)
		if(extension[i] == ' ')
			extension[i] = 0;
	char dot[2]; dot[0] = '.'; dot[1] = 0;
	strcat(buf, filename); 
	if(extension[0] == ' ' || (filename[0] == '.' && filename[1] == 0) || (filename[0] == '.' && filename[1] == '.' && filename[2] == 0))
		return buf;
	else
	{
		strcat(buf, dot);
		strcat(buf, extension);
	}
	return buf;
}

//EFM File Entry to string
//recommended buf length is 128 bytes
char* efm_fentry2str(char* buf, EFM_FileEntry* fe)
{
	char buffer[64];
	buf[0] = (fe->attributes & 16) ? 'd' : '-';
	buf[1] = (fe->permissions & 256) ? 'r' : '-'; buf[2] = (fe->permissions & 128) ? 'w' : '-'; buf[3] = (fe->permissions & 64) ? 'x' : '-';
	buf[4] = (fe->permissions & 32) ? 'r' : '-'; buf[5] = (fe->permissions & 16) ? 'w' : '-'; buf[6] = (fe->permissions & 8) ? 'x' : '-';
	buf[7] = (fe->permissions & 4) ? 'r' : '-'; buf[8] = (fe->permissions & 2) ? 'w' : '-'; buf[9] = (fe->permissions & 1) ? 'x' : '-';
	buf[10] = ' '; buf[11] = '1'; buf[12] = ' '; buf[13] = '\0';
	snprintf(buffer, 64, "%u ", fe->owner); strcat(buf, buffer);
	snprintf(buffer, 64, "%u ", fe->group); strcat(buf, buffer);
	snprintf(buffer, 64, "    %u ", fe->fileSize); strcat(buf, buffer);
	int hour, minute, year, month, day;
	hour = fe->mTime >> 11; minute = (fe->mTime >> 6) & 63;
	year = (fe->mDate >> 9) + 1980; month = (fe->mDate >> 5) & 7; day = fe->mDate & 31;
	snprintf(buffer, 64, "    %u/%u/%u ", year, month, day); strcat(buf, buffer);
	snprintf(buffer, 64, "%u:%u ", hour, minute); strcat(buf, buffer);
	snprintf(buffer, 64, "\t%s", fe->filename); strcat(buf, buffer);
	return buf;
}

void efm_readfiles(ep_window* w)
{
	TFFile *fp;
	EFM_FileEntry* fea = (EFM_FileEntry*)w->userdata[EFM_DIRENTRIES];

	puts("Files in /user:\n");
	fp = tf_fopen((uint8_t*)"/user", (const uint8_t*)"r");
	for(int i = 0; i < 256; i++)
	{
		FatFileEntry e;
		tf_fread((uint8_t*)&e, sizeof(FatFileEntry), fp);
		if(e.msdos.filename[0] == '\0')
		{
			w->userdata[EFM_DIRENTRIESC] = i;
			break;
		}
		if(e.msdos.filename[0] == ' ')
			continue;
		if(e.msdos.attributes & 0xf)
			continue;
		EFM_FileEntry* fe = &fea[i];
		efm_msdosfn2str(fe->filename, e.msdos.filename, (e.msdos.attributes & 16) ? NULL : e.msdos.extension);
		fe->attributes = e.msdos.attributes;
		fe->cTime = e.msdos.creationTime; fe->cDate = e.msdos.creationDate;
		fe->mTime = e.msdos.modifiedTime; fe->mDate = e.msdos.modifiedDate;
		fe->fileSize = e.msdos.fileSize;
		//placeholders
		fe->owner = 0; fe->group = 0;
		if(fe->attributes & 1) //if file is read-only
			fe->permissions = 0b100100100;
		else
			fe->permissions = 0b110110110;
	}
	tf_fclose(fp);
}

void efm_unload(ep_window* w)
{
	free((void*)w->userdata[EFM_DIRENTRIES]);
}

void efm_load(ep_window* w)
{
	strcpy(w->title, "Eelphant File Manager");
	w->x = vwidth() / 2 - 320;
	w->y = vheight() / 2 - 240;
	w->w = 720;
	w->h = 480;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
	w->userdata[EFM_SELECTED_FILE] = 0;
	w->userdata[EFM_DIRENTRIES] = (uint32_t)malloc(256 * sizeof(EFM_FileEntry));
	memset((void*)w->userdata[EFM_DIRENTRIES], 0, 256 * sizeof(EFM_FileEntry));
	efm_readfiles(w);
}

void efm_update(uint64_t dt, ep_window* w)
{

}

void efm_draw(int64_t bx, int64_t by, ep_window* w)
{
	int start = w->userdata[EFM_SELECTED_FILE];
	EFM_FileEntry* fe = (EFM_FileEntry*)w->userdata[1];
	vsetcol(0, 0, 0, 255);
	vd_print(bx + 10, by + 10, "PERM       L   O     G         SIZE      CREATION        FILENAME", NULL, NULL);
	vd_rectangle(FILL, bx + 10, by + 30, 700, 20);
	for(int i = start; i < w->userdata[EFM_DIRENTRIESC]; i++)
	{
		if(i == start + 20)
			break;
		char buffer[200];
		efm_fentry2str(buffer, &fe[i]);
		if(i == start)
			vsetcol(255, 255, 255, 255);
		vd_print(bx + 10, by + 30 + (i - start) * 16, buffer, NULL, NULL);
		if(i == start)
			vsetcol(0, 0, 0, 255);
	}
	vd_print(bx + 10, by + 460, "Page Up/Down - Move cursor | Enter - Open file | Delete - Delete file", NULL, NULL);
}

void efm_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
	EFM_FileEntry* fea = (EFM_FileEntry*)w->userdata[EFM_DIRENTRIES];
	if(ke)
	{
		if(ke->release)
		{
			switch(ke->keycode)
			{
				case 0x49: //cursor up
					if(w->userdata[EFM_SELECTED_FILE] > 0)
					{
						w->userdata[EFM_SELECTED_FILE]--;
					}
					else
					{
						w->userdata[EFM_SELECTED_FILE] = w->userdata[EFM_DIRENTRIESC] - 1;
					}
				break;
				case 0x51: //cursor down
					if(w->userdata[EFM_SELECTED_FILE] < w->userdata[EFM_DIRENTRIESC] - 1)
					{
						w->userdata[EFM_SELECTED_FILE]++;
					}
					else
					{
						w->userdata[EFM_SELECTED_FILE] = 0;
					}
				break;
				case 0x1C: //enter
				{
					char buffer[128]; buffer[0] = 0;
					strcat(buffer, "/user/");
					strcat(buffer, fea[w->userdata[EFM_SELECTED_FILE]].filename);
					printf("Opening %s in notepad\n", buffer);
					notepad_spawnf(buffer);
					break;
				}
				case 0x53: //delete
				{
					char buffer[128]; buffer[0] = 0;
					strcat(buffer, "/user/");
					strcat(buffer, fea[w->userdata[EFM_SELECTED_FILE]].filename);
					printf("Removing %s\n", buffer);
					tf_remove((uint8_t*)buffer);
					break;
				}
			}
		}
	}
}

void efm_spawn()
{
	ep_window* w = eelphant_create_window();
	if(!w)
	{
		printf("efm: can't allocate window\n");
		return;
	}
	w->load = &efm_load;
	w->unload = &efm_unload;
	w->update = &efm_update;
	w->draw = &efm_draw;
	w->load(w);
	for(int i = 0; i < 4; i++)
	{
		if(drives[i].letter == 0) continue;
		printf("Drive %c:\\ Size: %d MB %s\n", drives[i].letter, drives[i].type == 1 ? 0 : (drives[i].address.phys.size * 512 / 1024 / 1024), (drives[i].type == 0 ? "Physical" : (drives[i].type == 2 ? "Physical FAT32" : "Initrd")));
	}
	eelphant_switch_active(w);
	w->event = &efm_event;
}