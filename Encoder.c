#include "TM4C129.h"                    // Device header
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "timers.h"                     // ARM.FreeRTOS::RTOS:Timers
#include "Encoder.h"
int encoderCurrentValue=0;																						// Creamos una vairbale global tipo int
int encoderLastvalue=0;
int numeroDeFlancosDeSubida=0;
char flancos[5]="0";
int muestreo =0;
//varibles del timer para controlar el preriodo de muestreo
TimerHandle_t xTimer;																	// Llamamos a la variable xTimer
BaseType_t xTimerStartEncoder;															// Creamos una variable global tipo BaseType_t

void configurarEntradaEncoder(void){
	GPIOD_AHB->DEN |=0x01;																				// Declaramos como digital los pin 1 del puerto D
	GPIOD_AHB->DIR &=~0x01;																				// Declaramos como entrada los pin 1 del puerto D
	GPIOD_AHB->AFSEL &=~0x01;																			// Deshabilitamos funciones alternativas
	GPIOD_AHB->AMSEL &=~0x01;																			// Deshabilitamos funciones analógicas
	GPIOD_AHB->PCTL = ~0x01;																			// Omitimos funciones especiales
	GPIOD_AHB->PDR |= 0x01;																				// Habilitamos las resistencias Pull Down
}
void fin_de_muestreo(TimerHandle_t xTimer){
	muestreo=0;
}
int countFlancosDeSubidaEncoder(int time){
	numeroDeFlancosDeSubida=0;
	//configuracion del timmer
	//xTimer = xTimerCreate("PeriodoDeMuestreo",pdMS_TO_TICKS(time),pdFALSE,0,fin_de_muestreo);
	// Inicializamos el Timer a utilizar
	//xTimerStartEncoder = xTimerStart(xTimer,0);	
	//muestreo=1;
	//while(muestreo==1){
	for (int i=0;i<time;i++){
		if(encoderCurrentValue==1 && encoderLastvalue==0){
			numeroDeFlancosDeSubida++;
		}
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
	int copy=numeroDeFlancosDeSubida;
	return copy;
}


	