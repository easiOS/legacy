#ifndef H_PCI
#define H_PCI

#define PCI_PORT_CONF_ADDR 0xCF8
#define PCI_PORT_CONF_DATA 0xCFC

void pciinit(void);
void pci_ls(void);

unsigned char pci_cfg_read_b(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset);
void pci_cfg_write_b(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset, unsigned char val);

unsigned short pci_cfg_read_w(unsigned char bus, unsigned char slot, unsigned char func, unsigned char off);
void pci_cfg_write_w(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset, unsigned short val);

unsigned pci_cfg_read_dw(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset);
void pci_cfg_write_dw(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset, unsigned val);

#define PCIC_READ_VENDOR(b, s, f) pci_cfg_read_w(b, s, f, 0x00)
#define PCIC_READ_DEVICE(b, s, f) pci_cfg_read_w(b, s, f, 0x02)
#define PCIC_READ_CMD(b, s, f) pci_cfg_read_w(b, s, f, 0x04)
#define PCIC_READ_STATUS(b, s, f) pci_cfg_read_w(b, s, f, 0x06)
#define PCIC_READ_REVID(b, s, f) pci_cfg_read_b(b, s, f, 0x08)
#define PCIC_READ_CLASS(b, s, f) pci_cfg_read_dw(b, s, f, 0x08) >> 8 & 0xffffff
#define PCIC_READ_HDRT(b, s, f) pci_cfg_read_b(b, s, f, 0xe)
#define PCIC_READ_BAR(b, s, f, i) pci_cfg_read_dw(b, s, f, 0x10 + (i * 4))
#define PCIC_READ_INTL(b, s, f) pci_cfg_read_b(b, s, f, 0x3c)
#define PCIC_READ_INTP(b, s, f) pci_cfg_read_b(b, s, f, 0x3d)

#define PCIC_WRITE_VENDOR(b, s, f, v) pci_cfg_write_w(b, s, f, 0x00, v)
#define PCIC_WRITE_DEVICE(b, s, f, v) pci_cfg_write_w(b, s, f, 0x02, v)
#define PCIC_WRITE_CMD(b, s, f, v) pci_cfg_write_w(b, s, f, 0x04, v)
#define PCIC_WRITE_STATUS(b, s, f, v) pci_cfg_write_w(b, s, f, 0x06, v)
#define PCIC_WRITE_HDRT(b, s, f, v) pci_cfg_write_b(b, s, f, 0xe, v)
#define PCIC_WRITE_INTL(b, s, f, v) pci_cfg_write_b(b, s, f, 0x3c, v)
#define PCIC_WRITE_INTP(b, s, f, v) pci_cfg_write_b(b, s, f, 0x3d, v)

#endif /* H_PCI */