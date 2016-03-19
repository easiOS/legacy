#ifndef H_OHCI
#define H_OHCI

#include <stdint.h>

#define OHCI_REG_HCREVI 0x00 //Revision
#define OHCI_REG_HCCTRL 0x04 //Control
#define OHCI_REG_HCCMDS 0x08 //CommandStatus
#define OHCI_REG_HCINTS 0x0c //InterruptStatus
#define OHCI_REG_HCINTE 0x10 //InterruptEnable
#define OHCI_REG_HCINTD 0x14 //InterruptDisable
#define OHCI_REG_HCHCCA 0x18 //Host Controller Communications Area
#define OHCI_REG_HCPCED 0x1c //PeriodCurrentED
#define OHCI_REG_HCCHED 0x20 //ControlHeadED
#define OHCI_REG_HCCCED 0x24 //ControlCurrentED
#define OHCI_REG_HCBHED 0x28 //BulkHeadED
#define OHCI_REG_HCBCED 0x2c //BulkCurrentED
#define OHCI_REG_HCDH   0x30 //DoneHead
#define OHCI_REG_HCFMIN 0x34 //FmInterval
#define OHCI_REG_HCFMRE 0x38 //FmRemaining
#define OHCI_REG_HCFMNB 0x3c //FmNumber
#define OHCI_REG_HCPSTR 0x40 //PeriodicStart
#define OHCI_REG_HCLSTH 0x44 //LSThreshold
#define OHCI_REG_HCRHDA 0x48 //RhDescriptorA
#define OHCI_REG_HCRHDB 0x4c //RhDescriptorB
#define OHCI_REG_HCRHST 0x50 //RhStatus
#define OHCI_REG_HCRHPS 0x54 //RhPortStatus offset

struct ohci_reg_rev
{
	uint32_t reserved : 24;
	uint32_t version : 8;
} __attribute__((packed));

struct ohci_reg_ctrl
{
	uint32_t reserved : 22;
	uint32_t remote_wakeup_en : 1;
	uint32_t remote_wakeup_conn : 1;
	uint32_t introuting : 1;
	uint32_t functional_state : 2;
	uint32_t bulk_list_en : 1;
	uint32_t ctrl_list_en : 1;
	uint32_t isochrono_en : 1;
	uint32_t peri_list_en : 1;
	uint32_t ctrl_bulk_sr : 2;
} __attribute__((packed));

struct ohci_reg_cmdstatus
{
	uint32_t reserved2 : 14;
	uint32_t sched_ovr : 2;
	uint32_t reserved1 : 12;
	uint32_t ownship_req : 1;
	uint32_t bl_filled : 1;
	uint32_t cl_filled : 1;
	uint32_t hc_reset : 1;
} __attribute__((packed));

#define OHCI_REG_HCREV_11 0x11

void ohciinit(uint8_t bus, uint8_t slot);

#endif
