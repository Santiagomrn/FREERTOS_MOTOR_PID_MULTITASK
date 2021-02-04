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
#include "Nokia5110.h"
#include "Encoder.h"
#include "Botones.h"

void task0(void *pvParameters);
void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);
void delay(int iterations);
int calcularPid(int _offset, int _input );
int RPM_a_PPS(int _offset, int timeMs);
TaskHandle_t xPRINCIPAL;
TaskHandle_t xPANTALLA;
TaskHandle_t xMUESTREO;
//int offset=25;

int control=0;														//RPM MÁXIMO 4031
int userInput=4031;											//RPM mínimo 1232
//semaforos
SemaphoreHandle_t xSemaphorePantalla = NULL;

int main(void){
		SYSCTL->RCGCGPIO = 0x00007FFF;
		// Se configura puerto F(Leds)
	GPIOF_AHB -> DEN = 0x00FF;
	GPIOF_AHB -> DIR = 0x0FF;
	config_UART0();
	Nokia5110_Init();
	
	configurar_botones();
	configurarEntradaEncoder();
	PWM0A_Init(65535,65535*.5);
	
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
		
				//verifica si boton de Aumentar es presionado;
				if(botonAumentarPresionado()){				
						if(userInput<4031){   
							userInput=userInput+10;	
						}		
				}
				//verifica si boton de Disminuir es presionado;
				if(botonDisminuirPresionado()){	
						if(userInput>1700){
								userInput=userInput-10;			
						}
				}
				//imprimirMensaje("fin botones");
				vTaskDelay(75);
		}
}

//muestreo del encoder
void task1(void* arg){
	for(;;){
				
		//imprimirMensaje("Muestreo");
		//countFlancosDeSubidaEncoder(467545);
		
		countFlancosDeSubidaEncoder(2338);
		//imprimirMensaje("fin Muestreo");
		int flancos=getFlancos();																	//Se lee los pulsos en un periodo de 50ms
		control=calcularPid((userInput*.178/20),flancos);		//offset es el valor que se espera alcanzar en RPM, la funcion RPS sirve para transformar de RPM a PPS
		PWM0A_Duty(control);
		
		vTaskDelay(200);
	}
	
}

//muestra en pantalla
void task2(void* arg){
	for(;;){
		GPIOF_AHB->DATA ^= 0xFF;
		
				int flancos=getFlancos();
				char snum[10];
				// convert int to string [buf]
				sprintf(snum, "%d", flancos);
				imprimirMensaje("flancos: ");
				imprimirMensaje(snum);
	
		
				imprimirMensaje("control: ");
				sprintf(snum, "%d", control);
				imprimirMensaje(snum);
				imprimirMensaje("userInput: ");
				sprintf(snum, "%d", userInput);
				imprimirMensaje(snum);
				/*LcdClear();
		    char a[7]="RPS: ";
				a[5]=userInput/10+0x30;
				a[6]=userInput%10+0x30;
				Nokia5110_OutString(a);*/
				LcdClear();
				Nokia5110_OutString("RPM = ");
				sprintf(snum, "%d", userInput);
				Nokia5110_OutString(snum);
				//imprimirMensaje("fin pantalla");
		
		vTaskDelay(500);
	
	}
}
/*
//calculo del PID
void task3(void* arg){
	
	for(;;){
		 char snum[10];
		int flancos=getFlancos();																	//Se lee los pulsos en un periodo de 50ms
		control=calcularPid((offset*.178/20),flancos);		//offset es el valor que se espera alcanzar en RPM, la funcion RPS sirve para transformar de RPM a PPS
		
		sprintf(snum, "%d", control);
		imprimirMensaje("control: ");
		imprimirMensaje(snum);
		
										
		vTaskDelay(250);
		
	}
}*/

// VARIABLE GLOBAL [Error previo]
float Error_0=0; //error anterior  [Variable Global]
float Integral=65535*.49;

int calcularPid(int _offset, int _input ){
    float kp = 10; // Variable proporcional
    float ki = 2000; // Variable integral
    float kd = 10; // Variable diferencial
    int error = 0; // error actual
    float Proporcional =0;  
    float Derivativo=0;
    float control ;
    float T = 0.050; // Periodo de muestreo en Segundos [50ms]
		char snum[10];
    error = _offset - _input;													//_offset equivale a RPS a pulsos por segundo  _input pulsos por segundo
    Proporcional = error * kp;
    Integral = Integral + error * ki * T;
    Derivativo = (error - Error_0) * kd / T;
    Error_0 = error;
			sprintf(snum, "%d", error);
		imprimirMensaje("error: ");
		imprimirMensaje(snum);
		
    control = (int) (Proporcional+Integral+Derivativo);
    if(control > 65535){
        control = 65535*.99;
    }
		/*
		if(control<65535*.5){
				control=65535*.5;
		}*/
		
	
    return control;
}
int RPM_a_PPS(int _offset,int timeMs ){//rpm
    // Convertir un valor de RPM ingresado por el usuario a 
    // pulsos por segundo correspondiente al valor que debería obtenerse 
    // por parte del encoder
    // 334 pulsos equivale a 1 revolución

	  _offset = (_offset) * (360/1.07);
	
    // 0.05ms * 20 * 60 -> _input * 20 * 60
    // 1 min -> _input * 20 * 60
	
		
	return _offset;
}

