#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include <String.h>
void config_UART0(void){
        SYSCTL_RCGCUART_R  |= 0x01; // Activaci‹án del reloj UART0 /
        while((SYSCTL_PRUART_R &  0x01)==0){}; // Esperar activaci‹án /
        SYSCTL_RCGCGPIO_R |= 0x01; // Activa el reloj del puerto A / 
        while((SYSCTL_PRGPIO_R &  0x01)==0){}; // Esperar activaci‹án
        UART0_CTL_R &= ~0x01; // Se desactiva el UART0 Para configurar
        GPIO_PORTA_AFSEL_R |= 0x03; // Funci‹án especial
        GPIO_PORTA_DIR_R &= ~0x01; // PA0 entrada
        GPIO_PORTA_DIR_R |= 0x02; // PA0 entrada
        GPIO_PORTA_PUR_R &= ~0x03; // Sin resistencia PULL UP
        GPIO_PORTA_PDR_R &= ~0x03; // Sin resistencia PULL DOWN
     
        GPIO_PORTA_PCTL_R |= (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011; // 00010001
        GPIO_PORTA_DEN_R |= 0x03;
         
             
        UART0_IBRD_R = (UART0_IBRD_R & ~0xFFFF) + 104;
        UART0_FBRD_R = (UART0_FBRD_R & ~0x3F )+ 11; 
             
        // Configurar Trama
        UART0_LCRH_R = (UART0_LCRH_R & ~0xFF) | 0x70;   
        // 
        //UART0_CC_R |=0x00;
        UART0_CTL_R &= ~0x20; // HSE = 0
        UART0_CTL_R  |= 0x301; // UARTEN = 1, RXE = 1, TXE= 1
}
 
void writeUARTChar(char c){
    while( (UART0_FR_R & UART_FR_TXFF) != 0 );
    UART0_DR_R = c;   
}
void imprimirNuevalinea(){
	// Se imprime nueva lÌnea en terminal Putty
					writeUARTChar('\r');
					writeUARTChar('\n');
}
void imprimirMensaje(char *msg ){
				for(int i=0;i<strlen(msg);i++){
					writeUARTChar(msg[i]);
				}
				imprimirNuevalinea();
}	

//

uint32_t readUARTChar(void){
    char c;
    while(  (UART0_FR_R & UART_FR_RXFE)!= 0){}; // Se verifica que el FIFO no estπ» vac≥‡o
    c = UART0_DR_R; 
    return c;
     
}
 
 uint32_t readUARTLastChar(void){
    char a;

	

				 a=UART0_DR_R;//leo el ultimo cararcter del fifo en la UART
	  
   return a;
}
     
 
     
     
