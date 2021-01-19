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
int offset=98;//rpm

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
		imprimirMensaje("botones");
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
				imprimirMensaje("fin botones");
				vTaskDelay(50);
		}
}

//muestreo del encoder
void task1(void* arg){
	for(;;){
		imprimirMensaje("Muestreo");
		countFlancosDeSubidaEncoder(467545);
		imprimirMensaje("fin Muestreo");
		vTaskDelay(200);
	}
}

//muestra en pantalla
void task2(void* arg){
	for(;;){
		GPIOF_AHB->DATA ^= 0xFF;
		imprimirMensaje("pantalla");
				int flancos=getFlancos();
				char snum[10];
				// convert int to string [buf]
				sprintf(snum, "%d", flancos);
				imprimirMensaje(snum);
				sprintf(snum, "%d", offset);
				imprimirMensaje(snum);
			Nokia5110_OutString("revoluciones: 30");
			imprimirMensaje("fin pantalla");
		
		vTaskDelay(500);
	
	}
}

//calculo del PID
void task3(void* arg){
	for(;;){
		
		int control=calcularPid(RPM_a_PPS(offset,50),50);
		PWM0A_Duty(control);
		vTaskDelay(250);
	
	}
}

// VARIABLE GLOBAL [Error previo]
float Error_0=0; //error anterior  [Variable Global]
float Integral=0;

int calcularPid(int _offset, int _input ){
    float kp = 0.02; // Variable proporcional
    float ki = 0.7; // Variable integral
    float kd = 0.0006; // Variable diferencial
    float error = 0; // error actual
    float Proporcional =0;  
    float Derivativo=0;
    float control ;
    float T = 0.050; // Periodo de muestreo en Segundos [50ms]
    error = _offset - _input;
    Proporcional = error * kp;
    Integral = Integral + error * ki * T;
    Derivativo = (error - Error_0) * kd / T;
    Error_0 = error;
    control = (int) (Proporcional+Integral+Derivativo);
    if(control > 100){
        control = 100;
    }
    return control;
}
int RPM_a_PPS(int _offset,int timeMs ){//rpm
    // Convertir un valor de RPM ingresado por el usuario a 
    // pulsos por segundo correspondiente al valor que debería obtenerse 
    // por parte del encoder
    // 334 pulsos equivale a 1 revolución
    _offset = (_offset/60) * (360/1.07);
    // 0.05ms * 20 * 60 -> _input * 20 * 60
    // 1 min -> _input * 20 * 60

	return _offset;
}

