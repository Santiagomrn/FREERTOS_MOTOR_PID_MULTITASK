#include <stdint.h>
#include <stdio.h> 
#include <String.h>
#include "TM4C129.h"                    // Device header
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "timers.h"                     // ARM.FreeRTOS::RTOS:Timers
#include "semphr.h"
#include "PLL.h"
#include "PWM.h"
#include "UART0.h"
#include "Encoder.h"
#include "Botones.h"

void task0(void *pvParameters);
void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);
void delay(int iterations);
TaskHandle_t xPRINCIPAL;
TaskHandle_t xPANTALLA;
TaskHandle_t xMUESTREO;
int offset=98;

//semaforos
SemaphoreHandle_t xSemaphorePantalla = NULL;

int main(void){
		SYSCTL->RCGCGPIO = 0x00007FFF;
		// Se configura puerto F(Leds)
	GPIOF_AHB -> DEN = 0x00FF;
	GPIOF_AHB -> DIR = 0x0FF;
	config_UART0();
	
	configurar_botones();
	configurarEntradaEncoder();
	PWM0A_Init(65535,65535/2);
	
	//crear semaforo
	xSemaphorePantalla = xSemaphoreCreateMutex();
	
	// Crear prototipo de la tarea 
	xTaskCreate (task0, "tareaPrincipal", configMINIMAL_STACK_SIZE, NULL, 0, &xPRINCIPAL );
	// Crear prototipo de tarea
	xTaskCreate (task1, "muestreoDelEncoder", configMINIMAL_STACK_SIZE, NULL, 2, &xMUESTREO );
	xTaskCreate (task2, "muestraInofrmacionEnPantalla", configMINIMAL_STACK_SIZE, NULL, 1, &xPANTALLA );
	//xTaskCreate (task3, "PID", configMINIMAL_STACK_SIZE, NULL, 1, NULL );

		// Arrancar scheduler 
	vTaskStartScheduler();
}	

// Tarea Principal
void task0(void* arg){
		for(;;){
		imprimirMensaje("taks0");
				//verifica si boton de Aumentar es presionado;
				if(botonAumentarPresionado()){				
						if(offset<100){
							offset++;
						}		
				}
				//verifica si boton de Disminuir es presionado;
				if(botonDisminuirPresionado()){	
						if(offset>0){
							offset--;			
						}
				}
				imprimirMensaje("fin taks0");
				vTaskDelay(50);
		}
}

//muestreo del encoder
void task1(void* arg){
	for(;;){
		imprimirMensaje("taks1");
		countFlancosDeSubidaEncoder(467545);
		imprimirMensaje("fin taks1");
		vTaskDelay(200);
	}
}

//muestra en pantalla
void task2(void* arg){
	for(;;){
		GPIOF_AHB->DATA ^= 0xFF;
		imprimirMensaje("taks2");
				int flancos=getFlancos();
				char snum[10];
				// convert int to string [buf]
				sprintf(snum, "%d", flancos);
				imprimirMensaje(snum);
				sprintf(snum, "%d", offset);
				imprimirMensaje(snum);
			imprimirMensaje("fin taks2");
		vTaskDelay(500);
	
	}
}

//calculo del PID
void task3(void* arg){
	for(;;){
		vTaskDelay(250);
	
	}
}

void delay(int iterations){
 for (int i=0;i<=iterations;i++){
	 
 }
		
}

