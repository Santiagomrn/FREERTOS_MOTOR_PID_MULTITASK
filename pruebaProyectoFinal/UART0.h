#include <stdint.h>
#include "tm4c1294ncpdt.h"

uint32_t readUARTLastChar(void);
void config_UART0(void);
void writeUARTChar(char c);
uint32_t readUARTChar(void);

