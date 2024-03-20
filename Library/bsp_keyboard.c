#include "bsp_keyboard.h"

//! Private Variable
static uint32_t key_tick;
static uint8_t  key_idx;

//! Public Variable
uint8_t  key_val;          //存放当前按下按键的键值

//! Private Function
void key_exti_mode()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void key_input_mode()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  
}

void key_get_idx(uint16_t GPIO_Pin)
{
  //PA4~7为中断模式时,获取行号
  key_idx=(uint8_t)GPIO_Pin;
  //PA0~3为Input模式时,获取列坐标
  key_input_mode();
  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET){key_idx |= 0x01;}//0b 0000 0001
  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_RESET){key_idx |= 0x02;}//0b 0000 0010
  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==GPIO_PIN_RESET){key_idx |= 0x04;}//0b 0000 0100
  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)==GPIO_PIN_RESET){key_idx |= 0x08;}//0b 0000 1000
  key_exti_mode();
}

void key_get_val()
{
  switch(key_idx)
  {
		case 0x11:key_val='A';break;
		case 0x21:key_val='B';break;
		case 0x41:key_val='C';break; 
		case 0x12:key_val='3';break;
		case 0x22:key_val='6';break;
		case 0x42:key_val='9';break;
		case 0x14:key_val='2';break;
		case 0x24:key_val='5';break;
		case 0x44:key_val='8';break;
		case 0x28:key_val='4';break;
		case 0x81:key_val='D';break;
		case 0x82:key_val='#';break;
		case 0x84:key_val='0';break;
		case 0x88:key_val='*';break;
		case 0x18:key_val='1';break;
		case 0x48:key_val='7';break;
    default:key_val=0;break;
  }
  key_idx=0;
}

//! Public Function
void bsp_keyboard_init()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // //PC2独立按键GPIO管脚初始化, 中断模式
  // GPIO_InitStruct.Pin = GPIO_PIN_2;
  // GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  // GPIO_InitStruct.Pull = GPIO_NOPULL;
  // HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  //按键阵列PA0~3为开漏输出模式,输出低电平
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //按键阵列PA4~7为下降沿检测中断模式,因为PA0~3输出低电平
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //独立按键中断使能和优先级配置
  // HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
  // HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  // 按键阵列中断使能和优先级配置
  #if BSP_USE_FREERTOS
  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  #else
  HAL_NVIC_SetPriority(EXTI4_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 0);  
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  #endif

  key_idx=0;
  key_val=0;

  // println("keys init end");
}

void bsp_keyboard_scan(uint16_t pin)
{
  // key_tick为上一次按键检测结束的时刻,如果间隔太短则推出本次检测
  if((HAL_GetTick()-key_tick)<150)
    return;

  key_get_idx(pin);
  key_get_val();

  // 本次检测结束时记录时刻
  key_tick=HAL_GetTick();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{ 
  if(GPIO_Pin>=GPIO_PIN_4 && GPIO_Pin<=GPIO_PIN_7)
  {
    // 按键中断处理
    bsp_keyboard_scan(GPIO_Pin);
  }
  else
  {
    // 预留
    UNUSED(GPIO_Pin);
  }
}

