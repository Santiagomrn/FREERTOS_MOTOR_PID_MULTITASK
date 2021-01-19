/*
Equipo: 
	Alejandro García Cetina
	Santiago Miguel Rodríguez Noh
	Antonio Maldonado Pinzón 
	Wacha Espadas
	
	Link del video :OOO
*/

#include <stdint.h>
#include <String.h>
#include "TM4C129.h"                    // Device header
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "PLL.h"
#include "PWM.h"

#include "timers.h"                     // ARM.FreeRTOS::RTOS:Timers

//----------------------------------------------------Variables globales-------------------------------------------------------//
int banderaPeriodoMuestreoEncoder  = 0;
TimerHandle_t xTimer;																			  // Creamos una variable global tipo TimerHandle_t								
BaseType_t xTimerStartEncoder;															// Creamos una variable global tipo BaseType_t
int flag_encoder;																						// Creamos una vairbale global tipo int
extern TimerHandle_t xTimer;																	// Llamamos a la variable xTimer
extern void encoder_time(TimerHandle_t xTimer);								// Llamamos a la función del tiempo del encoder	
void task0(void *pvParameters);
void task1(void *pvParameters);
void configJ(void);
void imprimirMensajeTarea1(void);
void imprimirMensajeTarea2(void);
void imprimirNuevalinea(void);
// Configuración puerto J
void configJ(void){
	SYSCTL_RCGCGPIO_R |=0X0100; //HABILITA EL PUERTO J
	while ((SYSCTL_PRGPIO_R&0X0100)==0){};
	GPIO_PORTJ_AFSEL_R &=~0x03; //coloca un zero en la posicion 0 y 1 del registro GPIO_POTJ_AFSEL_R para controlarlo con los registros GPIO
	GPIO_PORTJ_DIR_R &=~0X03; ////coloca un zero en la posicion 0 y 1 del registro GPIO_POTJ_DIR_R para configurarlo como entrada
														//0 entrada			1 salidad
	GPIO_PORTJ_DEN_R |=0X03; ////coloca un UNO en la posicion 0 y 1 del registro 	GPIO_PORTJ_DEN_R para configurar como digital
														//0 analogica 1 digital 
	
	GPIO_PORTJ_PUR_R |=0X03;// COLOCO UN UNO en la posicion 0 y 1 del registro 	GPIO_PORTJ_PUR_R PARA CONFIGURAR resistencias pull up
}

void imprimirMensaje(char *msg ){
	// impresión de mensaje correspondiente a la tarea 1. 
	//		char msg[27] = "Tarea 0 Interruptor 1 en_";
	// se imprime el mensaje por carácter
	
				for(int i=0;i<strlen(msg);i++){
					writeUARTChar(msg[i]);
				}
}

//
void imprimirNuevalinea(){
	// Se imprime nueva línea en terminal Putty
					writeUARTChar('\r');
					writeUARTChar('\n');
}
int main(){
	SYSCTL->RCGCGPIO = 0x00007FFF;
	// Se configura puerto N(Leds)
	GPION-> DEN = 0x00FF;
	GPION->DIR = 0x0FF;
		// Se configura puerto F(Leds)
	GPIOF_AHB -> DEN = 0x00FF;
	GPIOF_AHB -> DIR = 0x0FF;
	
	// Configurar la UART0
	config_UART0();
	// Se inicializa el puerto J
	configJ();

	// Crear prototipo de la tarea 
	xTaskCreate (task0, "primera", configMINIMAL_STACK_SIZE, NULL, 0, NULL );
	// Crear prototipo de tarea
	xTaskCreate (task1, "segunda", configMINIMAL_STACK_SIZE, NULL, 1, NULL );

	// Arrancar scheduler 
	vTaskStartScheduler();
}
//------------------------------------------Función que es llamada para el timer-----------------------------------------------//
void encoder_time(TimerHandle_t xTimer){
	banderaPeriodoMuestreoEncoder = 1;
	GPION->DATA^=0x01;																				// Verificamos que la tarea cumple
	flag_encoder = 1;																					// Determinamos valor para flag_encoder
		//imprimirMensajeTarea1();
		//writeUARTChar('X');
		//imprimirNuevalinea();
}

	// Función de la tarea
void task0(void* arg){

		for(;;){
				// retardo de 1s
			//vTaskDelay(500);
			GPIOF_AHB->DATA ^= 0xFF;
			// Leer interruptor
			while(1){
				// Si está presionado se manda 0 con el mensaje de la tarea 0
					imprimirMensaje("Hello world Hello world");
					//writeUARTChar('9');
					imprimirNuevalinea();
			}

		}
}
void task1(void* arg){
	for(;;){
		banderaPeriodoMuestreoEncoder = 0;
		vTaskDelay(1000);
		GPION->DATA ^= 0xFF;
		// Leer interruptor
			//if((GPIOJ_AHB->DATA & 0x02)== 0x00){
				// Se manda un 0, con el mensaje de la tarea 1
		xTimer = xTimerCreate("Encoder_timer",pdMS_TO_TICKS(500),pdFALSE,0,encoder_time);
		// Inicializamos el Timer a utilizar
		xTimerStartEncoder = xTimerStart(xTimer,0);		
		while(banderaPeriodoMuestreoEncoder == 0){
				//imprimirMensajeTarea2();
					//writeUARTChar('5');
					//imprimirNuevalinea();
			//}
		}
	}
}
