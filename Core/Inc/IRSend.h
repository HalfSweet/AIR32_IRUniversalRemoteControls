#ifndef IRSEND_H_
#define IRSEND_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "u8g2.h"

void delay_us(uint16_t nus);
void SendNEC(uint8_t *data);
void SendUniversal(uint16_t *data,uint16_t size);

#ifdef __cplusplus
}
#endif

#endif