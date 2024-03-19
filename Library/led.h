#ifndef __LED_H__
#define __LED_H__

#include "stm32f1xx_hal.h"

#define 	LED_PORT 	GPIOC		
#define 	LED_R			GPIO_PIN_6
#define 	LED_G			GPIO_PIN_7	
#define 	LED_B			GPIO_PIN_8
#define 	LED_ON			GPIO_PIN_SET
#define 	LED_OFF		GPIO_PIN_RESET

void led_switch(uint16_t color, GPIO_PinState state);
void led_blink(uint16_t color, uint8_t time, uint32_t interval);//ms

#endif