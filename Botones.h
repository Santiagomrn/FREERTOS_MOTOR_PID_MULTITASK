// Botones.h
#include <stdint.h>
#include <stdint.h>
#include <String.h>
#include "TM4C129.h"                    // Device header
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core

void configurar_botones(void);
int botonAumentarPresionado(void);
int botonDisminuirPresionado(void);