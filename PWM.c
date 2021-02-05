#include <stdint.h>
#include <stdint.h>
#include <String.h>
#include "TM4C129.h"                    // Device header
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "PLL.h"

// F0 -> IN1 
// GND -> IN2

// Inicialización del PWM
void PWM0A_Init(uint16_t period, uint16_t duty){
	// Se configura el puerto F
	SYSCTL->RCGCGPIO = 0x00007FFF;
	// Se habilita el puerto N
	GPION-> DEN = 0x00FF; // Digital Enable
	GPION->DIR = 0x0FF;  // Dir
	// Se hace uso del registro de configuración para PWM
	SYSCTL->RCGCPWM |= 0x00000001;
	SYSCTL->RCGCGPIO |= 0x00000020;

	// Se verifica el puerto F
	while((SYSCTL->PRGPIO&0x00000020)==0){};
	GPIOF_AHB->AFSEL|=0x01; 		//Pin F0 salida del PWM. Selección de función especial
	GPIOF_AHB->DEN|=0x01;			// Digital enable
	
	GPIOF_AHB->PCTL=(GPIOF_AHB->PCTL&0xFFFFFFF0)+0x00000006; // Se configura el PCTL para PWM
	GPIOF_AHB->AMSEL&=~0x01;	// Función  analógica
	
	while((SYSCTL->PRPWM&0x00000001)==0){}; // Se espera la habilitación del puerto
	// Configuración del PWM 
	PWM0->CC |= 0x00000100;	//
	PWM0->CC &= 0x00000007; 
	PWM0->CC += 0x00000000;
	PWM0->_0_CTL = 0;
	PWM0->_0_GENA = (0x000000C0|0x00000008);
	
	PWM0->_0_LOAD=period-1; // Se carga el periodo de la señal PWM
	PWM0->_0_CMPA=duty-1;	// Se carga el ciclo de trabajo del PWM
	PWM0->_0_CTL|=0x00000001; // Uso del registro de control PWM
	PWM0->ENABLE|=0x00000001; // Se habilita el PWM

}

void PWM0A_Duty(uint16_t duty){
	// Se actualiza el ciclo de trabajo como
	PWM0->_0_CMPA =duty-1;
}

