#ifndef H_E100
#define H_E100

#include <stdint.h>

/* Control / Status Register byte offsets - SDM Table 11 */
enum CSROffsets {
	SCBStatus=0,             SCBCmd=2,              SCBPointer = 4,
	CSRPort=8,               CSRFlash=12,           CSREeprom = 14,
	CSRCtrlMDI=16,           CSREarlyRx=20
};

/* System Control Block Command Word - SDM Table 12 */
enum SCBCmdBits {
	/* SCB Interrupt Masks - SDM Table 14 */
	SCBMaskCmdDone=0x8000,   SCBMaskRxDone=0x4000,  SCBMaskCmdIdle=0x2000,
	SCBMaskRxSuspend=0x1000, SCBMaskEarlyRx=0x0800, SCBMaskFlowCtl=0x0400,
	SCBTriggerIntr=0x0200,   SCBMaskAll=0x0100,
	/* SCB Control Commands - SDM Table 14-16 */
	CUStart=0x0010,          CUResume=0x0020,       CUStatsAddr=0x0040,
	CUShowStats=0x0050,      CUCmdBase=0x0060,      CUDumpStats=0x0070,
	RUStart=0x0001,          RUResume=0x0002,       RUAbort=0x0004,
	RUAddrLoad=0x0006,       RUResumeNoResources=0x0007
};

enum SCBPortCmds {
	PortReset=0, PortSelfTest=1, PortPartialReset=2, PortDump=3
};

/* Action Commands - SDM Table 14,37 */
enum ActionCommands {
	CmdNOp = 0,              CmdIASetup = 1,        CmdConfigure = 2,
	CmdMulticastList = 3,    CmdTx = 4,             CmdTDR = 5,
	CmdDump = 6,             CmdDiagnose = 7,
	/* And some extra flags: */
	CmdEndOfList = 0x8000,
	CmdSuspend = 0x4000,     CmdIntr = 0x2000,      CmdTxFlex = 0x0008
};

enum TCBBits {
	TCB_C=0x8000,            TCB_OK=0x2000,         TCB_U=0x1000
};

enum RFDBits {
	/* Status Word Bits */
	RFDRxCol=0x0001,         RFDIAMatch=0x0002,     RFDNoMatch=0x0004,
	RFDReserved3=0x0008,     RFDRxErr=0x0010,       RFDEthType=0x0020,
	RFDReserved6=0x0040,     RFDShort=0x0080,       RFDDMAOverrun=0x0100,
	RFDNoBufs=0x0200,        RFDCRCAlign=0x0400,    RFDCRCError=0x0800,
	RFDReserved12=0x1000,    RFD_OK=0x2000,         RFDComplete=0x8000,
	/* Command Word Bits */
	//RFD_SF=0x0008,           RFDSuspend=0x4000,     RFDEndOfList=0x8000,
	/* Other */
	RFDMaskCount=0x3FFF
};

enum phy_chips {
	NonSuchPhy=0,            I82553AB,              I82553C,
	I82503,                  DP83840,               S80C240,
	S80C24,                  PhyUndefined,          DP83840A=10
};

struct e100_private {
	
};

void e100init(uint8_t bus, uint8_t slot);

#endif
