#include <stdint.h>
#include "tm4c1294ncpdt.h"

void PWM0A_Init(uint16_t period, uint16_t duty){
		SYSCTL->RCGCGPIO = 0x00007FFF;

	GPION-> DEN = 0x00FF;
	GPION->DIR = 0x0FF;
	
	SYSCTL->RCGCPWM |= 0x00000001;
	SYSCTL->RCGCGPIO |= 0x00000020;


	while((SYSCTL->PRGPIO&0x00000020)==0){};
	GPIOF_AHB->AFSEL|=0x01;
	GPIOF_AHB->DEN|=0x01;
	
	GPIOF_AHB->PCTL=(GPIOF_AHB->PCTL&0xFFFFFFF0)+0x00000006;
	GPIOF_AHB->AMSEL&=~0x01;
	
	while((SYSCTL->PRPWM&0x00000001)==0){};
	PWM0->CC |= 0x00000100;
	PWM0->CC &= 0x00000007;
	PWM0->CC += 0x00000000;
	PWM0->_0_CTL = 0;
	PWM0->_0_GENA = (0x000000C0|0x00000008);
	
	PWM0->_0_LOAD=period-1;
	PWM0->_0_CMPA=duty-1;
	PWM0->_0_CTL|=0x00000001;
	PWM0->ENABLE|=0x00000001;

}

void PWM0A_Duty(uint16_t duty){
	PWM0->_0_CMPA =duty-1;
	GPION->DATA ^= 0xFF;
}
void PWM0B_Init(uint16_t period, uint16_t duty){
	SYSCTL->RCGCPWM |= 0x00000001;
	SYSCTL->RCGCGPIO |= 0x00000020;


	while((SYSCTL->PRGPIO&0x00000020)==0){};
	GPIOF_AHB->AFSEL|=0x02;
	GPIOF_AHB->DEN|=0x02;
	
	GPIOF_AHB->PCTL=(GPIOF_AHB->PCTL&0xFFFFFF0F)+0x00000060;
	GPIOF_AHB->AMSEL&=~0x02;
	
	while((SYSCTL->PRPWM&0x00000001)==0){};
	PWM0->CC |= 0x00000100;
	PWM0->CC &= 0x00000007;
	PWM0->CC += 0x00000000;
	PWM0->_0_CTL = 0;
	PWM0->_0_GENB = (0x00000C00|0x00000008);
	
	PWM0->_0_LOAD=period-1;
	PWM0->_0_CMPB=duty-1;
	PWM0->_0_CTL|=0x00000001;
	PWM0->ENABLE|=0x00000002;

}

// change duty cycle of PWM0B/PF1
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0B_Duty(uint16_t duty){
	PWM0->_0_CMPB =duty-1;
}