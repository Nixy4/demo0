#include "led.h"

void led_switch(uint16_t color, GPIO_PinState state)
{
	HAL_GPIO_WritePin( LED_PORT, color, state  );
}

void led_blink(uint16_t color, uint8_t time, uint32_t interval)
{
	for(int i=0; i<time ; i++ )
	{
		HAL_GPIO_WritePin( LED_PORT, color, LED_ON  );
		HAL_Delay( interval );
		HAL_GPIO_WritePin( LED_PORT, color, LED_OFF  );
		HAL_Delay( interval );
	}
}