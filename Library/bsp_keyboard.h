#ifndef BSP_KEYBOARD_H
#define BSP_KEYBOARD_H

//! BSP配置文件
#include "bsp_conf.h"

//! Pubulic Variable
extern uint8_t key_val;

//! Public Function
void bsp_keyboard_init(void);
void bsp_keyboard_scan(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif // !BSP_KEYBOARD_H