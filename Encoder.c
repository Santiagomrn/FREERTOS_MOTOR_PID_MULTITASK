#include "TM4C129.h"                    // Device header
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "timers.h"                     // ARM.FreeRTOS::RTOS:Timers
#include "Encoder.h"
// Valor que almacena el valor actual de los pulsos del encoder del motor
int encoderCurrentValue=0;			// Creamos una vairbale global tipo int
int encoderLastvalue=0; 			// Se almacenan los pulsos previos del encoder
int numeroDeFlancosDeSubida=0;		// Alamacena el número de flancos del encoder
char flancos[5]="0";				// Variable auxiliar de flancos 
int muestreo =0;					// Se inicializa muestreo
//varibles del timer para controlar el preriodo de muestreo

//D0 -> CANAL A o B

TimerHandle_t xTimer;																	// Llamamos a la variable xTimer
BaseType_t xTimerStartEncoder;															// Creamos una variable global tipo BaseType_t

void configurarEntradaEncoder(void){
	GPIOD_AHB->DEN |=0x01;																				// Declaramos como digital los pin 1 del puerto D
	GPIOD_AHB->DIR &=~0x01;																				// Declaramos como entrada los pin 1 del puerto D
	GPIOD_AHB->AFSEL &=~0x01;																			// Deshabilitamos funciones alternativas
	GPIOD_AHB->AMSEL &=~0x01;																			// Deshabilitamos funciones anal�gicas
	GPIOD_AHB->PCTL = ~0x01;																			// Omitimos funciones especiales
	GPIOD_AHB->PDR |= 0x01;																				// Habilitamos las resistencias Pull Down
}
// Se finaliza el tiempo de muestreo, reseteando el número de flancos almacenados 
void fin_de_muestreo(TimerHandle_t xTimer){
	muestreo=0;
}
// Función que cuenta el número de flancos por parte del encoder
int countFlancosDeSubidaEncoder(int time){
	// Número de flancos de subida 
	numeroDeFlancosDeSubida=0;
	// La tarea habilita el muestreo de los flancos del encoder
	for (int i=0;i<time;i++){
		// Se verifica flanco de subida
		if(encoderCurrentValue==1 && encoderLastvalue==0){
			// Se incrementa el número de fllancos
			numeroDeFlancosDeSubida++;
		}
		// Se guarda el valor previo
		encoderLastvalue=encoderCurrentValue;
		//actualizo el valor actual del encoder
		if ((GPIOD_AHB->DATA&0x01)==0x01){
			encoderCurrentValue=1; //alto
		}else{
			encoderCurrentValue=0;//bajo
		}
	}
}

int getFlancos(void){
	// Se hace una copia de los flancos para evitar inconsistencia de datos
	int copy=numeroDeFlancosDeSubida;
	// Se retorna la copia
	return copy;
}


	