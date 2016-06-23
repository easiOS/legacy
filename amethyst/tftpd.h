#ifndef H_TFTPD
#define H_TFTPD

#define TFTP_RRQ 1
#define TFTP_WRQ 2
#define TFTP_DAT 3
#define TFTP_ACK 4
#define TFTP_ERR 5

int tftpd_spawn(int argc, char** argv);

#endif /* H_TFTPD */