#include "IRSend.h"

#define DLY_TIM_Handle (&htim1)
void delay_us(uint16_t nus)
{
    __HAL_TIM_SET_COUNTER(DLY_TIM_Handle, 0);
    __HAL_TIM_ENABLE(DLY_TIM_Handle);
    while (__HAL_TIM_GET_COUNTER(DLY_TIM_Handle) < nus)
    {
        ;
    }
    __HAL_TIM_DISABLE(DLY_TIM_Handle);
}

void SendNEC(uint8_t *data)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    delay_us(9000);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
    delay_us(4500);

    for (int i = 0; i < 32; i++)
    {
        if (data[i / 8] & (1 << (7 - (i % 8))))
        {
            HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
            delay_us(560);
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
            delay_us(1690);
        }
        else
        {
            HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
            delay_us(560);
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
            delay_us(560);
        }
    }

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    delay_us(563);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}

void SendUniversal(uint16_t *data, uint16_t size)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    delay_us(9000);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
    delay_us(data[0]);

    if (size % 2 == 0) // 最后一位串扰进去了，减去
        size -= 1;
    for (int i = 1; i < size; i++)
    {
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
        delay_us(data[i]);
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
        delay_us(data[++i]);
    }

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    delay_us(563);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}