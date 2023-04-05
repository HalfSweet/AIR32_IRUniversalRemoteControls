#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "u8g2.h"

    typedef struct
    {
        uint8_t size;
        uint16_t data[256];
    } IRRemoteData_t;

    typedef enum
    {
        IR_STATE_IDLE,    // 空闲状态
        IR_STATE_RECEIVE, // 接收数据状态
        IR_STATE_UNIVERSAL_IDLE, // 万能遥控器空闲状态
        IR_STATE_UNIVERSAL_RECEIVE, // 万能遥控器接收数据状态
        IR_STATE_FINISH,  // 接收完成状态
    } IRState_t;

    typedef enum
    {
        NEC,      // NEC协议
        Almighty, // 万能遥控器
    } ReceiveMode_t;

    const uint16_t IR_START_HIGH_LEVEL_TIME = 4500;   // 4500us
    const uint16_t IR_START_LOW_LEVEL_TIME = 9000;    // 9000us
    const uint16_t IR_LOGIC_0_HIGH_LEVEL_TIME = 560;  // 560us
    const uint16_t IR_LOGIC_0_LOW_LEVEL_TIME = 560;   // 560us
    const uint16_t IR_LOGIC_1_HIGH_LEVEL_TIME = 1680; // 1680us
    const uint16_t IR_LOGIC_1_LOW_LEVEL_TIME = 560;   // 560us

    const uint16_t IR_ERROR_TIME = 200; // 误差时间,200us

#define IR_TIM htim4
#define IR_TIM_CHANNE TIM_CHANNEL_3

    void IRInit(void);
    void IRDeInit(void);

    /**
     * @brief IR捕获回调函数
     *
     */
    void IRCaptureCallback(void);

    /**
     * @brief 定时器溢出回调函数
     *
     */
    void IRPeriodElapsedCallback(void);

    void NECDecode(IRRemoteData_t *data);

    extern volatile IRState_t IRState;
    extern uint8_t IRFlag;
    extern IRRemoteData_t IRRemoteData;
    extern ReceiveMode_t ReceiveMode;
    extern uint8_t NECRemoteData[4];

    extern u8g2_t u8g2;

#ifdef __cplusplus
}
#endif

#endif