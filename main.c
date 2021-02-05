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

// Prototipos de tareas
void task0(void *pvParameters);
void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);
// prototipo de funciones
void delay(int iterations);
int calcularPid(int _offset, int _input );
int RPM_a_PPS(int _offset, int timeMs);
// Excepciones de tareas
TaskHandle_t xPRINCIPAL;
TaskHandle_t xPANTALLA;
TaskHandle_t xMUESTREO;

int control=0;			//RPM M�XIMO 4031
int userInput=4031;		//RPM m�nimo 1232
//semaforos
SemaphoreHandle_t xSemaphorePantalla = NULL;

int main(void){
	// Se configura el puerto
	SYSCTL->RCGCGPIO = 0x00007FFF;
	// Se configura puerto F(Leds)
	GPIOF_AHB -> DEN = 0x00FF;
	GPIOF_AHB -> DIR = 0x0FF;
	// Se configura la UART (Putty como pantalla alternativa)
	config_UART0();
	// Se inicializa la pantalla Nokia 5110
	Nokia5110_Init();
	// se configuran botonnes	
	configurar_botones();
	// Se configura el puerto D para que sea la entrada del encoder
	configurarEntradaEncoder();
	// Se inicializa el PWM con un 50% del ciclo de trabajo
	PWM0A_Init(65535,65535*.5);
	
	//crear semaforo
	xSemaphorePantalla = xSemaphoreCreateMutex();
	
	// Crear prototipo de la tarea 
	xTaskCreate (task0, "tareaPrincipal", configMINIMAL_STACK_SIZE, NULL, 0, &xPRINCIPAL );
	// Crear prototipo de tarea
	xTaskCreate (task1, "muestreoDelEncoder", configMINIMAL_STACK_SIZE, NULL, 2, &xMUESTREO );
	// Crear prototipo de tarea
	xTaskCreate (task2, "muestraInofrmacionEnPantalla", configMINIMAL_STACK_SIZE, NULL, 1, &xPANTALLA );

	// Arrancar scheduler 
	vTaskStartScheduler();
}	

// Tarea Principal
void task0(void* arg){
		for(;;){
		
				//verifica si boton de Aumentar es presionado;
				if(botonAumentarPresionado()){				
						if(userInput<4031){   
							// Incrementar las RPM en 10
							userInput=userInput+10;	
						}		
				}
				//verifica si boton de Disminuir es presionado;
				if(botonDisminuirPresionado()){	
						if(userInput>1700){
							// Disminuir las RPM en 10
								userInput=userInput-10;			
						}
				}
				// Tiempo designado para la tarea
				vTaskDelay(75);
		}
}

//muestreo del encoder
// Se realizsa el muestreo del encoder en el Pin D
void task1(void* arg){
	for(;;){
		// Se invoca la función contar flancos en un determinado tiempo
		countFlancosDeSubidaEncoder(2338);
		// Se obtienen los flancos leídos (pulsos)
		int flancos=getFlancos();								//Se lee los pulsos en un periodo de tiempo determinado
		// Se hace el cálculo del PID con 
		control=calcularPid((userInput*.178/20),flancos);		//offset es el valor que se espera alcanzar en RPM, la funcion RPS sirve para transformar de RPM a PPS
		// Se ajusta el ciclo de trabajo respecto a lo que arroja el PID 
		PWM0A_Duty(control);
		// Duración de la tarea
		vTaskDelay(200);
	}
	
}

//muestra en pantalla
void task2(void* arg){
	for(;;){
		// Se configura el pin F 
		GPIOF_AHB->DATA ^= 0xFF;
			// Se obtienen los flancos actuales
				int flancos=getFlancos();
				// Variable que almacena el dato a transmitir en PUTTY (Segunda pantalla)
				char snum[10];
				// convert int to string [buf]
				sprintf(snum, "%d", flancos);
				// Mensajes que se imprimen en PUTTY (Segunda pantalla)
				imprimirMensaje("flancos: ");
				imprimirMensaje(snum);
				imprimirMensaje("control: ");
				sprintf(snum, "%d", control);
				imprimirMensaje(snum);
				imprimirMensaje("userInput: ");
				sprintf(snum, "%d", userInput);
				imprimirMensaje(snum);

				// Se Limpia la pantalla NOKIA 5110 
				LcdClear();
				// Se muestra la palabra RPM
				Nokia5110_OutString("RPM = ");
				// Se castea el valor decimal a String
				sprintf(snum, "%d", userInput);
				// Se muestra el valor actual al que el usuario quiere llegar en RPM
				Nokia5110_OutString(snum);
		// duración de la tarea
		vTaskDelay(500);
	
	}
}

// VARIABLE GLOBAL [Error previo]
float Error_0=0; //error anterior  [Variable Global]
float Integral=65535*.49; // Variable de integración

int calcularPid(int _offset, int _input ){
    float kp = 10; // Variable proporcional
    float ki = 2000; // Variable integral
    float kd = 10; // Variable diferencial
    int error = 0; // error actual
    float Proporcional =0;  // Se reestablece el valor proporcional
    float Derivativo=0;	// Se reestablece el valor derivativo
    float control ; // Variable donde se almacenará el nuevo valor del Duty Cycle
    float T = 0.050; // Periodo de muestreo en Segundos [50ms]

	char snum[10]; // Variable que almacena el valor a imprimir en el debbugger Putty
    error = _offset - _input;	//_offset equivale a RPS a pulsos por segundo  _input pulsos por segundo
    Proporcional = error * kp; // Variable proporcional (PID)
    Integral = Integral + error * ki * T; // Variable Integral (PID)
    Derivativo = (error - Error_0) * kd / T; // Variable derivativa (PID)
    Error_0 = error; // Se almacena el valor previo
		// Impresión en Putty
		sprintf(snum, "%d", error);
		imprimirMensaje("error: ");
		imprimirMensaje(snum);
	// Se hace el cálculo del nuevo Duty Cycle
    control = (int) (Proporcional+Integral+Derivativo);
	// Se limita valores superiores mapeados al máximo número de registro
    if(control > 65535){
        control = 65535*.99;
    }

    return control;
}
int RPM_a_PPS(int _offset,int timeMs ){//rpm
// Se hace la conversión de RPM a pulsos en un segundo
	  _offset = (_offset) * (360/1.07);
		
	return _offset;
}

