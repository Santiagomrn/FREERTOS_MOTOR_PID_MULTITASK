#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "PLL.h"

void PLL_Init(void){
	uint32_t timeout;
	
	SYSCTL->RSCLKCFG &= ~0x10000000;
	
	SYSCTL->MOSCCTL &= ~(0x00000004|0x00000008);
	
	while((SYSCTL->RIS&0x00000100)==0){};
		
	SYSCTL->RSCLKCFG = (SYSCTL->RSCLKCFG&~0x0F000000)+0x03000000;
		
	SYSCTL->RSCLKCFG = (SYSCTL->RSCLKCFG&~0x0F000000)+0x03000000;
		
	SYSCTL->DSCLKCFG = (SYSCTL->DSCLKCFG&~0x00F00000)+0x00300000;
		
	#define FXTAL 25000000  // fixed, this crystal is soldered to the Connected Launchpad
	#define Q            0
	#define N            4  // chosen for reference frequency within 4 to 30 MHz
	#define MINT        96  // 480,000,000 = (25,000,000/(0 + 1)/(4 + 1))*(96 + (0/1,024))
	#define MFRAC        0  // zero to reduce jitter
  //    SysClk = fVCO / (PSYSDIV + 1)
	#define SYSCLK (FXTAL/(Q+1)/(N+1))*(MINT+MFRAC/1024)/(PSYSDIV+1)
		
		SYSCTL->PLLFREQ0 = (SYSCTL->PLLFREQ0&~0x000FFC00)+(MFRAC<<10) |
											 (SYSCTL->PLLFREQ0&~0x000003FF)+(MINT<<0);
		
		SYSCTL->PLLFREQ1 = (SYSCTL->PLLFREQ1&~0x00001F00)+(Q<<8) |
											 (SYSCTL->PLLFREQ1&~0x0000001F)+(N<<0);
		
		SYSCTL->PLLFREQ0 |= 0x00800000;
		SYSCTL->RSCLKCFG |=	0x40000000;
		
		if(SYSCLK < 16000000){
    // FBCHT/EBCHT = 0, FBCE/EBCE = 0, FWS/EWS = 0
    SYSCTL->MEMTIM0 = (SYSCTL->MEMTIM0&~0x03EF03EF) + (0x0<<22) + (0x0<<21) + (0x0<<16) + (0x0<<6) + (0x0<<5) + (0x0);
  } else if(SYSCLK == 16000000){
    // FBCHT/EBCHT = 0, FBCE/EBCE = 1, FWS/EWS = 0
    SYSCTL->MEMTIM0 = (SYSCTL->MEMTIM0&~0x03EF03EF) + (0x0<<22) + (0x1<<21) + (0x0<<16) + (0x0<<6) + (0x1<<5) + (0x0);
  } else if(SYSCLK <= 40000000){
    // FBCHT/EBCHT = 2, FBCE/EBCE = 0, FWS/EWS = 1
    SYSCTL->MEMTIM0 = (SYSCTL->MEMTIM0&~0x03EF03EF) + (0x2<<22) + (0x0<<21) + (0x1<<16) + (0x2<<6) + (0x0<<5) + (0x1);
  } else if(SYSCLK <= 60000000){
    // FBCHT/EBCHT = 3, FBCE/EBCE = 0, FWS/EWS = 2
    SYSCTL->MEMTIM0 = (SYSCTL->MEMTIM0&~0x03EF03EF) + (0x3<<22) + (0x0<<21) + (0x2<<16) + (0x3<<6) + (0x0<<5) + (0x2);
  } else if(SYSCLK <= 80000000){
    // FBCHT/EBCHT = 4, FBCE/EBCE = 0, FWS/EWS = 3
    SYSCTL->MEMTIM0 = (SYSCTL->MEMTIM0&~0x03EF03EF) + (0x4<<22) + (0x0<<21) + (0x3<<16) + (0x4<<6) + (0x0<<5) + (0x3);
  } else if(SYSCLK <= 100000000){
    // FBCHT/EBCHT = 5, FBCE/EBCE = 0, FWS/EWS = 4
    SYSCTL->MEMTIM0 = (SYSCTL->MEMTIM0&~0x03EF03EF) + (0x5<<22) + (0x0<<21) + (0x4<<16) + (0x5<<6) + (0x0<<5) + (0x4);
  } else if(SYSCLK <= 120000000){
    // FBCHT/EBCHT = 6, FBCE/EBCE = 0, FWS/EWS = 5
    SYSCTL->MEMTIM0 = (SYSCTL->MEMTIM0&~0x03EF03EF) + (0x6<<22) + (0x0<<21) + (0x5<<16) + (0x6<<6) + (0x0<<5) + (0x5);
  } else{
    // A setting is invalid, and the PLL cannot clock the system faster than 120 MHz.
    // Skip the rest of the initialization, leaving the system clocked from the MOSC,
    // which is a 25 MHz crystal.
    return;
  }
	
	timeout = 0;
	
	while(((SYSCTL->PLLSTAT & 0x00000001) == 0) && (timeout < 0xFFFF)){
    timeout = timeout + 1;
  }
	
	if(timeout == 0xFFFF){
    // The PLL never locked or is not powered.
    // Skip the rest of the initialization, leaving the system clocked from the MOSC,
    // which is a 25 MHz crystal.
    return;
  }
  // 9)Write the SYSCTL_RSCLKCFG_R register's PSYSDIV value, set the USEPLL bit to
  //   enabled, and set the MEMTIMU bit.
  SYSCTL->RSCLKCFG = (SYSCTL->RSCLKCFG&~0x000003FF)+(PSYSDIV&0x000003FF) |
                       0x80000000 |
                       0x10000000;
		
		

}