#include "IRRemote.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "main.h"

IRRemoteData_t IRRemoteData;
uint8_t NECRemoteData[4];
uint8_t IRFlag = 0;                         // IR标志位，存储接收到多少个bit
volatile IRState_t IRState = IR_STATE_IDLE; // IR状态机

uint8_t FirstFlag = 0; // 第一次进入中断标志位

uint8_t count = 0;
uint8_t high_count = 0;
uint8_t low_count = 0;

uint8_t testArr[4];

void IRInit(void)
{
    __HAL_TIM_SET_CAPTUREPOLARITY(&IR_TIM, IR_TIM_CHANNE, TIM_INPUTCHANNELPOLARITY_FALLING); // 改为下降沿
    FirstFlag = 0;                                                                           // 第一次进入中断标志位清零
    HAL_TIM_Base_Start_IT(&IR_TIM);                                                          // 启动定时器
    HAL_TIM_IC_Start_IT(&IR_TIM, IR_TIM_CHANNE);                                             // 启动输入捕获
    myPrintf("IR init\r\n");
}
void IRDeInit(void)
{
    IRFlag = 0;                                       // IR标志位，存储接收到多少个bit
    __HAL_TIM_SET_COUNTER(&IR_TIM, 0);                // 计数器清零
    __HAL_TIM_SET_COMPARE(&IR_TIM, IR_TIM_CHANNE, 0); // 比较值清零
    HAL_TIM_IC_Stop_IT(&IR_TIM, IR_TIM_CHANNE);       // 停止输入捕获
    HAL_TIM_Base_Stop_IT(&IR_TIM);                    // 停止定时器
    IRState = IR_STATE_IDLE;
}

void IRCaptureCallback(void)
{
    static uint8_t CaptureFlag = 0; // 捕获标志位
    uint16_t CaptureBuf = 0;        // 存放捕获缓存

    count++;

    if (CaptureFlag == 1) // 上升沿捕获
    {
        low_count++;
        CaptureBuf = HAL_TIM_ReadCapturedValue(&IR_TIM, IR_TIM_CHANNE); // 获取当前的捕获值.
        __HAL_TIM_SET_COUNTER(&IR_TIM, 0);
        __HAL_TIM_SET_CAPTUREPOLARITY(&IR_TIM, IR_TIM_CHANNE, TIM_ICPOLARITY_FALLING); // 设置为下降沿捕获
        CaptureFlag = 0;

        switch (IRState)
        {
        case IR_STATE_UNIVERSAL_RECEIVE:
            IRFlag++;
            IRRemoteData.size = IRFlag;
            IRRemoteData.data[IRRemoteData.size - 1] = CaptureBuf;
            break;

        default:
            break;
        }
    }
    else
    {
        high_count++;
        CaptureBuf = HAL_TIM_ReadCapturedValue(&IR_TIM, IR_TIM_CHANNE); // 获取当前的捕获值.
        __HAL_TIM_SET_COUNTER(&IR_TIM, 0);
        __HAL_TIM_SET_CAPTUREPOLARITY(&IR_TIM, IR_TIM_CHANNE, TIM_INPUTCHANNELPOLARITY_RISING); // 改为上升沿
        CaptureFlag = 1;

        if (FirstFlag == 0) // 第一次进入中断，得到的数据无意义
        {
            FirstFlag = 1;
            return;
        }

        switch (IRState)
        {
        case IR_STATE_IDLE:
            if (CaptureBuf >= IR_START_HIGH_LEVEL_TIME - IR_ERROR_TIME && CaptureBuf <= IR_START_HIGH_LEVEL_TIME + IR_ERROR_TIME)
            {
                IRState = IR_STATE_RECEIVE; // 进入接收状态
                myPrintf("IR receive start\r\n");
            }

            break;

        case IR_STATE_UNIVERSAL_IDLE:
            // if (CaptureBuf >= IR_START_HIGH_LEVEL_TIME - IR_ERROR_TIME && CaptureBuf <= IR_START_HIGH_LEVEL_TIME + IR_ERROR_TIME)
            // {
            //     IRState = IR_STATE_RECEIVE; // 进入接收状态
            //     //IRState = IR_STATE_UNIVERSAL_RECEIVE; // 进入接收状态
            //     // IRFlag = 1;
            //     //  IRFlag++;
            //     //  IRRemoteData.size = IRFlag;
            //     //  IRRemoteData.data[IRRemoteData.size - 1] = CaptureBuf;
            //     myPrintf("IR UNIVERSAL\r\n");
            // }
            if (CaptureBuf >= IR_START_HIGH_LEVEL_TIME - IR_ERROR_TIME && CaptureBuf <= IR_START_HIGH_LEVEL_TIME + IR_ERROR_TIME)
            {
                IRState = IR_STATE_UNIVERSAL_RECEIVE; // 进入接收状态
                                                      // IRFlag = 1;
                IRFlag++;
                IRRemoteData.size = IRFlag;
                IRRemoteData.data[IRRemoteData.size - 1] = CaptureBuf;
                myPrintf("IR un\r\n");
            }
            break;

        case IR_STATE_RECEIVE:
            if (CaptureBuf >= IR_LOGIC_1_HIGH_LEVEL_TIME - IR_ERROR_TIME && CaptureBuf <= IR_LOGIC_1_HIGH_LEVEL_TIME + IR_ERROR_TIME)
            {
                IRFlag++;
                NECRemoteData[IRFlag / 8] |= (0x01 << (7 - (IRFlag % 8)));
            }
            else if (CaptureBuf >= IR_LOGIC_0_HIGH_LEVEL_TIME - IR_ERROR_TIME && CaptureBuf <= IR_LOGIC_0_HIGH_LEVEL_TIME + IR_ERROR_TIME)
            {
                IRFlag++;
                NECRemoteData[IRFlag / 8] &= ~(0x01 << (7 - (IRFlag % 8)));
            }
            else
            {
                IRFlag = 0;
                IRState = IR_STATE_IDLE;                          // 进入空闲状态
                __HAL_TIM_SET_COUNTER(&IR_TIM, 0);                // 计数器清零
                __HAL_TIM_SET_COMPARE(&IR_TIM, IR_TIM_CHANNE, 0); // 比较值清零
                myPrintf("IR receive error\r\n");
                return;
            }
            break;

        case IR_STATE_UNIVERSAL_RECEIVE:
            IRFlag++;
            IRRemoteData.size = IRFlag;
            IRRemoteData.data[IRRemoteData.size - 1] = CaptureBuf;
            // if (CaptureBuf >= IR_LOGIC_1_HIGH_LEVEL_TIME - IR_ERROR_TIME && CaptureBuf <= IR_LOGIC_1_HIGH_LEVEL_TIME + IR_ERROR_TIME)
            // {
            //     IRFlag++;
            //     NECRemoteData[IRFlag / 8] |= (0x01 << (7 - (IRFlag % 8)));
            // }
            // else if (CaptureBuf >= IR_LOGIC_0_HIGH_LEVEL_TIME - IR_ERROR_TIME && CaptureBuf <= IR_LOGIC_0_HIGH_LEVEL_TIME + IR_ERROR_TIME)
            // {
            //     IRFlag++;
            //     NECRemoteData[IRFlag / 8] &= ~(0x01 << (7 - (IRFlag % 8)));
            // }
            // else
            // {
            //     IRFlag = 0;
            //     IRState = IR_STATE_UNIVERSAL_RECEIVE;             // 进入空闲状态
            //     __HAL_TIM_SET_COUNTER(&IR_TIM, 0);                // 计数器清零
            //     __HAL_TIM_SET_COMPARE(&IR_TIM, IR_TIM_CHANNE, 0); // 比较值清零
            //     myPrintf("IR receive error\r\n");
            //     return;
            // }
            break;

        default:
            break;
        }
    }
}

void IRPeriodElapsedCallback(void)
{
    // myPrintf("count = %d\r\n", count);
    // myPrintf("low_count = %d\r\n", low_count);
    // myPrintf("high_count = %d\r\n", high_count);
    switch (IRState)
    {
    case IR_STATE_RECEIVE: // 接收状态，溢出了说明数据接受完了
        myPrintf("IR_STATE_RECEIVE\r\n");
        myPrintf("Receive bit num = %d\r\n", IRFlag);
        IRFlag = 0;
        IRState = IR_STATE_FINISH;                  // 进入完成状态
        HAL_TIM_IC_Stop_IT(&IR_TIM, IR_TIM_CHANNE); // 停止输入捕获
        HAL_TIM_Base_Stop_IT(&IR_TIM);              // 停止定时器
        myPrintf("IR receive finish\r\n");
        break;

    case IR_STATE_UNIVERSAL_RECEIVE:
        myPrintf("Receive bit num = %d\r\n", IRFlag);
        // myPrintf("IR receive data: 0x%02x 0x%02x 0x%02x 0x%02x\r\n", testArr[0], testArr[1], testArr[2], testArr[3]);
        IRFlag = 0;
        IRState = IR_STATE_FINISH;                  // 进入完成状态
        HAL_TIM_IC_Stop_IT(&IR_TIM, IR_TIM_CHANNE); // 停止输入捕获
        HAL_TIM_Base_Stop_IT(&IR_TIM);              // 停止定时器
        myPrintf("IR receive finish\r\n");
        break;

    case IR_STATE_IDLE: // 空闲状态，溢出了说明没有接收到数据

        IRFlag = 0;
        IRState = IR_STATE_IDLE;                          // 进入空闲状态
        __HAL_TIM_SET_COUNTER(&IR_TIM, 0);                // 计数器清零
        __HAL_TIM_SET_COMPARE(&IR_TIM, IR_TIM_CHANNE, 0); // 比较值清零
        return;

    case IR_STATE_UNIVERSAL_IDLE:
        IRFlag = 0;
        IRState = IR_STATE_UNIVERSAL_IDLE;                // 进入空闲状态
        __HAL_TIM_SET_COUNTER(&IR_TIM, 0);                // 计数器清零
        __HAL_TIM_SET_COMPARE(&IR_TIM, IR_TIM_CHANNE, 0); // 比较值清零
        return;
    default:
        break;
    }
    __HAL_TIM_SET_COUNTER(&IR_TIM, 0);                // 计数器清零
    __HAL_TIM_SET_COMPARE(&IR_TIM, IR_TIM_CHANNE, 0); // 比较值清零
}

void NECDecode(IRRemoteData_t *data)
{
    if (data->size == 4) // 判断是否接收到4个字节
    {
        if (data->data[0] == ~data->data[1] && data->data[2] == ~data->data[3]) // 判断是否为NEC协议
        {
            myPrintf("IR receive data: 0x%02x 0x%02x 0x%02x 0x%02x\r\n", NECRemoteData[0], NECRemoteData[1], NECRemoteData[2], NECRemoteData[3]);
        }
        else // 接收到的数据不是NEC协议
        {
            // 报错
        }
    }
    else // 接收到的数据不是4个字节，说明不是NEC协议
    {
        // 报错
    }
}