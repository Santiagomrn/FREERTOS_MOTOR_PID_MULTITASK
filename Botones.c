#include <stdint.h>
#include <stdint.h>
#include <String.h>
#include "TM4C129.h"                    // Device header
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "Botones.h"

void configurar_botones(){
    SYSCTL->RCGCGPIO |= 0x0100;//HABILITA EL PUERTO J
    while ((SYSCTL->RCGCGPIO&0X0100)==0){};
    GPIOJ_AHB->AFSEL &=~0x03; //coloca un zero en la posicion 0 y 1 del registro GPIO_POTJ_AFSEL_R para controlarlo con los registros GPIO
		GPIOJ_AHB->DIR &=~0x03;////coloca un zero en la posicion 0 y 1 del registro GPIO_POTJ_DIR_R para configurarlo como entrada
																										//0 entrada            1 salidad
		GPIOJ_AHB->DEN  |=0X03; ////coloca un zero en la posicion 0 y 1 del registro GPIO_POTJ_DIR_R para configurarlo como entrada
																										//0 entrada            1 salidad
    GPIOJ_AHB->PUR |=0x03;// COLOCO UN UNO en la posicion 0 y 1 del registro     GPIO_PORTJ_PUR_R PARA CONFIGURAR resistencias pull up
}
int botonAumentarPresionado(){
    if( (GPIOJ_AHB->DATA & 0x01) == 0x01){
        return 0;
    }else{
        return 1;
    }
}
int botonDisminuirPresionado(){
     if( (GPIOJ_AHB->DATA & 0x02) == 0x02){
        return 0;
    }else{
        return 1;
    }
}