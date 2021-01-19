#include <stdint.h>

void PWM0A_Init(uint16_t period, uint16_t duty);

// change duty cycle of PWM0A/PF0
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0A_Duty(uint16_t duty);
void PWM0B_Init(uint16_t period, uint16_t duty);

// change duty cycle of PWM0A/PF0
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0B_Duty(uint16_t duty);
