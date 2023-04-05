/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "IRRemote.h"
#include "u8g2.h"
#include "stdio.h"
#include "string.h"
#include "menu.h"
#include "SDdriver.h"
#include "IRSend.h"
#include <stdlib.h>
#include "ff.h" // 包含FATFS库头文件
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);

u8g2_t u8g2;

volatile uint8_t KeyState[4];

void ShowStartScreen(void);
void ScanKeyboard(void);

void ResetState(void);
void ResetKeyboard(void);
// uint8_t test_sd =0;	//用于测试格式�?
uint8_t write_cnt = 0; // 写SD卡次�?

static uint8_t ArrowFlag = 0; // 用于指示选项
static uint8_t PageFlag = 0;  // 用来指示是第几页（一页4个选项）
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
ReceiveMode_t ReceiveMode;
FATFS fs;
FIL file;
uint8_t SDCardRes = 0;
UINT Bw;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM4_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_TIM2_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    MX_TIM3_Init();
    MX_TIM1_Init();
    MX_FATFS_Init();

    /* Initialize interrupts */
    MX_NVIC_Init();
    /* USER CODE BEGIN 2 */
    myPrintf("Start\r\n");
    HAL_TIM_Base_Start_IT(&htim3);
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8g2_gpio_and_delay_stm32);
    u8g2_InitDisplay(&u8g2);     // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    ShowStartScreen();
    HAL_Delay(1000);

    Get_SDCard_Capacity();
    OpenSD();
    // MenuBasic();
    //  IRInit();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        static uint32_t RunTime = 0;
        switch (MenuMode)
        {
        case MENU_BASIC:
            MenuBasic();
            MenuMode = MENU_BASIC_SELECT;
            break;

        case MENU_BASIC_SELECT:

            if (KeyState[1] == 1) // 按下�??
            {
                KeyState[1] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag >= 3)
                {
                    ArrowFlag = 0;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * 3, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                else
                {
                    ArrowFlag++;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag - 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[0] == 1) // 按上�??
            {
                KeyState[0] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag <= 0)
                {
                    // ArrowFlag = 3;
                    ArrowFlag = 0;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                else
                {
                    ArrowFlag--;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag + 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[2] == 1) // 确认
            {
                KeyState[2] = 0;
                switch (ArrowFlag)
                {
                case 0:

                    ArrowFlag = 0;
                    RunTime = HAL_GetTick();
                    MenuNECLearn();
                    ResetState();
                    MenuMode = MENU_NEC_LEARN;
                    break;
                case 1:

                    IRState = IR_STATE_UNIVERSAL_RECEIVE;
                    ArrowFlag = 0;
                    RunTime = HAL_GetTick();
                    MenuUniversalLearn();
                    ResetState();
                    MenuMode = MENU_UNIVERSAL_LEARN;
                    break;
                case 2:

                    ArrowFlag = 0;
                    MenuSend();
                    ResetState();
                    MenuMode = MENU_SEND;
                    break;
                case 3:

                    ArrowFlag = 0;
                    MenuRecordView();
                    ResetState();
                    MenuMode = MENU_Record_View;
                    break;
                default:
                    break;
                }
            }

            u8g2_SendBuffer(&u8g2);

            break;

        case MENU_NEC_LEARN:
            IRInit();
            while (IRState != IR_STATE_FINISH)
            {
                HAL_Delay(10);
                if (KeyState[3] == 1) // 取消
                {
                    KeyState[3] = 0;
                    IRDeInit();
                    ResetState();
                    goto MENU_NEC_LEARN_END;
                }
                if (HAL_GetTick() - RunTime > 5 * 1000) // 5s学习时间，超时退出
                {
                    myPrintf("Time out, tick is %d, runtime is %d\r\n", HAL_GetTick(), RunTime);
                    IRDeInit();
                    ResetState();
                    goto MENU_NEC_LEARN_END;
                }
            }
            IRState = IR_STATE_IDLE;
            u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_chinese3);
            u8g2_DrawUTF8(&u8g2, (128 - 6 * 4) / 2, 32, "PASS");
            char temp[32];
            sprintf(temp, "0x%02X 0x%02X 0x%02X 0x%02X", NECRemoteData[0], NECRemoteData[1], NECRemoteData[2], NECRemoteData[3]);
            u8g2_DrawUTF8(&u8g2, (128 - 6 * 20), 48, temp);
            u8g2_SendBuffer(&u8g2);

            while (KeyState[2] != 1 && KeyState[3] != 1)
            {
                ;
            }
            myPrintf("Key is pressed\r\n");
            if (KeyState[2] == 1) // 确认
            {
                KeyState[2] = 0;
                MenuMode = MENU_SAVE_NEC;
                ArrowFlag = 0;
                MenuSaveNEC(0);
            }
            else if (KeyState[3] == 1) // 取消
            {
                KeyState[3] = 0;
                MenuMode = MENU_BASIC;
                ArrowFlag = 0;
            }
        MENU_NEC_LEARN_END:
            break;

        case MENU_UNIVERSAL_LEARN:
            IRState = IR_STATE_UNIVERSAL_IDLE;
            IRInit();

            while (IRState != IR_STATE_FINISH)
            {
                HAL_Delay(10);
                if (KeyState[3] == 1) // 取消
                {
                    KeyState[3] = 0;
                    IRDeInit();
                    ResetState();
                    goto MENU_NEC_LEARN_END;
                }
                if (HAL_GetTick() - RunTime > 5 * 1000) // 5s学习时间，超时退出
                {
                    IRDeInit();
                    ResetState();
                    goto MENU_UNIVERSAL_LEARN_END;
                }
            }
            IRState = IR_STATE_IDLE;
            u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_chinese3);
            u8g2_DrawUTF8(&u8g2, (128 - 6 * 4) / 2, 32, "PASS");
            u8g2_SendBuffer(&u8g2);

            while (KeyState[2] != 1 && KeyState[3] != 1)
            {
                ;
            }
            myPrintf("Key is pressed\r\n");
            if (KeyState[2] == 1) // 确认
            {
                KeyState[2] = 0;
                ArrowFlag = 0;
                MenuMode = MENU_SAVE_UNIVERSAL;
                ArrowFlag = 0;
                MenuSaveNEC(0);
            }
            else if (KeyState[3] == 1) // 取消
            {
                KeyState[3] = 0;
                ArrowFlag = 0;
                MenuMode = MENU_BASIC;
                ArrowFlag = 0;
            }
        MENU_UNIVERSAL_LEARN_END:
            break;

        case MENU_SAVE_NEC:
            // if (KeyState[1] == 1) // 按下�??
            // {
            //     KeyState[1] = 0;

            //     u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
            //     if (ArrowFlag >= 3)
            //     {
            //         PageFlag++;
            //         ArrowFlag = 0;
            //         u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
            //         u8g2_SetDrawColor(&u8g2, 0);
            //         u8g2_DrawBox(&u8g2, 0, 12 + 12 * 3, 16, 16);
            //         u8g2_SetDrawColor(&u8g2, 1);
            //         MenuSaveNEC(PageFlag);
            //     }
            //     else
            //     {
            //         ArrowFlag++;
            //         u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
            //         u8g2_SetDrawColor(&u8g2, 0);
            //         u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag - 1), 16, 16);
            //         u8g2_SetDrawColor(&u8g2, 1);
            //     }
            //     myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            // }
            // else if (KeyState[0] == 1) // 按上�??
            // {
            //     KeyState[0] = 0;

            //     u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
            //     if (ArrowFlag <= 0)
            //     {
            //         ArrowFlag = 3;
            //         if (PageFlag > 0)
            //         {
            //             PageFlag--;
            //         }
            //         u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
            //         u8g2_SetDrawColor(&u8g2, 0);
            //         u8g2_DrawBox(&u8g2, 0, 12, 16, 16);
            //         u8g2_SetDrawColor(&u8g2, 1);
            //         MenuSaveNEC(PageFlag);
            //     }
            //     else
            //     {
            //         ArrowFlag--;
            //         u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
            //         u8g2_SetDrawColor(&u8g2, 0);
            //         u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag + 1), 16, 16);
            //         u8g2_SetDrawColor(&u8g2, 1);
            //     }
            //     myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            // }
            // else if (KeyState[2] == 1) // 确认
            // {
            //     KeyState[2] = 0;
            //     FIL file;
            //     int res;
            //     u8g2_ClearDisplay(&u8g2);
            //     u8g2_SetFont(&u8g2, IR_Air32_font_12);
            //     u8g2_DrawUTF8(&u8g2, (128 - 12 * 8) / 2, 12, "正在保存中......");
            //     u8g2_DrawUTF8(&u8g2, (128 - 12 * 5) / 2, 25, "请耐心等待");
            //     u8g2_SendBuffer(&u8g2);

            //     while (1)
            //     {

            //         char fileNameTemp[6];
            //         genRandomString(fileNameTemp, 6);
            //         char fileName[32];
            //         sprintf(fileName, "%s.IR", fileNameTemp); // 文件名
            //         myPrintf("Save file name is %s\r\n", fileName);
            //         res = f_open(&file, fileName, FA_READ);
            //         if (res == FR_OK)
            //         {
            //             f_close(&file); // 如果文件已存在，则重新生成文件名
            //             continue;
            //         }

            //         res = f_open(&file, fileName, FA_WRITE | FA_CREATE_ALWAYS);
            //         if (res != FR_OK)
            //         {
            //             myPrintf("f_open error\r\n");
            //             break;
            //         }
            //         break;
            //     }
            //     char temp[32];
            //     sprintf(temp, "NEC\r\n0x%02X 0x%02X 0x%02X 0x%02X\r\n", NECRemoteData[0], NECRemoteData[1], NECRemoteData[2], NECRemoteData[3]);
            //     res = f_write(&file, temp, strlen(temp), &Bw);
            //     if (res != FR_OK)
            //     {
            //         myPrintf("f_write error\r\n");
            //     }
            //     f_close(&file);

            //     u8g2_ClearDisplay(&u8g2);
            //     u8g2_SetFont(&u8g2, IR_Air32_font_12);
            //     u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "保存成功");
            //     u8g2_SendBuffer(&u8g2);
            //     HAL_Delay(1000);
            //     ResetState();
            //     MenuMode = MENU_NEC_LEARN;
            //     MenuNECLearn();
            //     RunTime = HAL_GetTick();
            // }
            // else if (KeyState[3] == 1) // 取消
            // {
            //     // KeyState[3] = 0;
            //     // MenuMode = MENU_BASIC;
            //     // ArrowFlag = 0;
            //     // PageFlag = 0;
            //     ResetState();
            // }
            {
                FIL file;
                int res;
                u8g2_ClearDisplay(&u8g2);
                u8g2_SetFont(&u8g2, IR_Air32_font_12);
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 8) / 2, 12, "正在保存中......");
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 5) / 2, 25, "请耐心等待");
                u8g2_SendBuffer(&u8g2);

                while (1)
                {

                    char fileNameTemp[6];
                    genRandomString(fileNameTemp, 6);
                    char fileName[32];
                    sprintf(fileName, "%s.IR", fileNameTemp); // 文件名
                    myPrintf("Save file name is %s\r\n", fileName);
                    res = f_open(&file, fileName, FA_READ);
                    if (res == FR_OK)
                    {
                        f_close(&file); // 如果文件已存在，则重新生成文件名
                        continue;
                    }

                    u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_chinese3);
                    char temp[32];
                    // sprintf(temp, "%s", fileName);
                    u8g2_DrawUTF8(&u8g2, (128 - 6 * 8) / 2, 36, fileName);
                    u8g2_SendBuffer(&u8g2);
                    HAL_Delay(500);

                    res = f_open(&file, fileName, FA_WRITE | FA_CREATE_ALWAYS);
                    if (res != FR_OK)
                    {
                        myPrintf("f_open error\r\n");
                        break;
                    }
                    break;
                }
                // char temp[32];
                sprintf(temp, "NEC\r\n0x%02X 0x%02X 0x%02X 0x%02X\r\n", NECRemoteData[0], NECRemoteData[1], NECRemoteData[2], NECRemoteData[3]);
                res = f_write(&file, temp, strlen(temp), &Bw);
                if (res != FR_OK)
                {
                    myPrintf("f_write error\r\n");
                }
                f_close(&file);

                u8g2_ClearDisplay(&u8g2);
                u8g2_SetFont(&u8g2, IR_Air32_font_12);
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "保存成功");
                u8g2_SendBuffer(&u8g2);
                HAL_Delay(1000);
                ResetState();
                MenuMode = MENU_NEC_LEARN;
                MenuNECLearn();
                RunTime = HAL_GetTick();

                u8g2_SendBuffer(&u8g2);
            }
            break;

        case MENU_SAVE_UNIVERSAL:
            // if (KeyState[1] == 1) // 按下�??
            // {
            //     KeyState[1] = 0;

            //     u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
            //     if (ArrowFlag >= 3)
            //     {
            //         PageFlag++;
            //         ArrowFlag = 0;
            //         u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
            //         u8g2_SetDrawColor(&u8g2, 0);
            //         u8g2_DrawBox(&u8g2, 0, 12 + 12 * 3, 16, 16);
            //         u8g2_SetDrawColor(&u8g2, 1);
            //         MenuSaveNEC(PageFlag);
            //     }
            //     else
            //     {
            //         ArrowFlag++;
            //         u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
            //         u8g2_SetDrawColor(&u8g2, 0);
            //         u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag - 1), 16, 16);
            //         u8g2_SetDrawColor(&u8g2, 1);
            //     }
            //     myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            // }
            // else if (KeyState[0] == 1) // 按上�??
            // {
            //     KeyState[0] = 0;

            //     u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
            //     if (ArrowFlag <= 0)
            //     {
            //         ArrowFlag = 3;
            //         if (PageFlag > 0)
            //         {
            //             PageFlag--;
            //         }
            //         u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
            //         u8g2_SetDrawColor(&u8g2, 0);
            //         u8g2_DrawBox(&u8g2, 0, 12, 16, 16);
            //         u8g2_SetDrawColor(&u8g2, 1);
            //         MenuSaveNEC(PageFlag);
            //     }
            //     else
            //     {
            //         ArrowFlag--;
            //         u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
            //         u8g2_SetDrawColor(&u8g2, 0);
            //         u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag + 1), 16, 16);
            //         u8g2_SetDrawColor(&u8g2, 1);
            //     }
            //     myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            // }
            // else if (KeyState[2] == 1) // 确认
            // {
            //     KeyState[2] = 0;
            //     FIL file;
            //     int res;
            //     u8g2_ClearDisplay(&u8g2);
            //     u8g2_SetFont(&u8g2, IR_Air32_font_12);
            //     u8g2_DrawUTF8(&u8g2, (128 - 12 * 8) / 2, 12, "正在保存中......");
            //     u8g2_DrawUTF8(&u8g2, (128 - 12 * 5) / 2, 25, "请耐心等待");
            //     u8g2_SendBuffer(&u8g2);

            //     while (1)
            //     {
            //         char fileNameTemp[6];
            //         genRandomString(fileNameTemp, 6);
            //         char fileName[32];
            //         sprintf(fileName, "%s.IR", fileNameTemp); // 文件名
            //         myPrintf("Save file name is %s\r\n", fileName);
            //         res = f_open(&file, fileName, FA_READ);
            //         if (res == FR_OK)
            //         {
            //             f_close(&file); // 如果文件已存在，则重新生成文件名
            //             continue;
            //         }

            //         res = f_open(&file, fileName, FA_WRITE | FA_CREATE_ALWAYS);
            //         if (res != FR_OK)
            //         {
            //             myPrintf("f_open error\r\n");
            //             break;
            //         }
            //         break;
            //     }

            //     char temp[32];
            //     sprintf(temp, "UNIVERSAL\r\n");
            //     res = f_write(&file, temp, strlen(temp), &Bw);
            //     if (res != FR_OK)
            //     {
            //         myPrintf("f_write error\r\n");
            //     }
            //     myPrintf("IRRemoteData.size = %d\r\n", IRRemoteData.size);
            //     sprintf(temp, "%d ", IRRemoteData.size);
            //     res = f_write(&file, temp, strlen(temp), &Bw);
            //     if (res != FR_OK)
            //     {
            //         myPrintf("f_write error\r\n");
            //     }
            //     for (size_t i = 0; i < IRRemoteData.size; i++)
            //     {
            //         sprintf(temp, "%d ", IRRemoteData.data[i]);
            //         res = f_write(&file, temp, strlen(temp), &Bw);
            //         if (res != FR_OK)
            //         {
            //             myPrintf("f_write error\r\n");
            //         }
            //     }
            //     f_close(&file);

            //     u8g2_ClearDisplay(&u8g2);
            //     u8g2_SetFont(&u8g2, IR_Air32_font_12);
            //     u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "保存成功");
            //     u8g2_SendBuffer(&u8g2);
            //     HAL_Delay(1000);
            //     ResetState();
            //     MenuMode = MENU_UNIVERSAL_LEARN;
            //     MenuUniversalLearn();
            //     RunTime = HAL_GetTick();
            // }
            // else if (KeyState[3] == 1) // 取消
            // {
            //     // KeyState[3] = 0;
            //     // MenuMode = MENU_BASIC;
            //     // ArrowFlag = 0;
            //     // PageFlag = 0;
            //     ResetState();
            // }

            // u8g2_SendBuffer(&u8g2);
            {
                FIL file;
                int res;
                u8g2_ClearDisplay(&u8g2);
                u8g2_SetFont(&u8g2, IR_Air32_font_12);
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 8) / 2, 12, "正在保存中......");
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 5) / 2, 25, "请耐心等待");
                u8g2_SendBuffer(&u8g2);

                while (1)
                {

                    char fileNameTemp[6];
                    genRandomString(fileNameTemp, 6);
                    char fileName[32];
                    sprintf(fileName, "%s.IR", fileNameTemp); // 文件名
                    myPrintf("Save file name is %s\r\n", fileName);
                    res = f_open(&file, fileName, FA_READ);
                    if (res == FR_OK)
                    {
                        f_close(&file); // 如果文件已存在，则重新生成文件名
                        continue;
                    }

                    u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_chinese3);
                    char temp[32];
                    // sprintf(temp, "%s", fileName);
                    u8g2_DrawUTF8(&u8g2, (128 - 6 * 8) / 2, 36, fileName);
                    u8g2_SendBuffer(&u8g2);
                    HAL_Delay(500);

                    res = f_open(&file, fileName, FA_WRITE | FA_CREATE_ALWAYS);
                    if (res != FR_OK)
                    {
                        myPrintf("f_open error\r\n");
                        break;
                    }
                    break;
                }
                // char temp[32];
                sprintf(temp, "UNIVERSAL\r\n");
                res = f_write(&file, temp, strlen(temp), &Bw);
                if (res != FR_OK)
                {
                    myPrintf("f_write error\r\n");
                }
                myPrintf("IRRemoteData.size = %d\r\n", IRRemoteData.size);
                sprintf(temp, "%d ", IRRemoteData.size);
                res = f_write(&file, temp, strlen(temp), &Bw);
                if (res != FR_OK)
                {
                    myPrintf("f_write error\r\n");
                }
                for (size_t i = 0; i < IRRemoteData.size; i++)
                {
                    sprintf(temp, "%d ", IRRemoteData.data[i]);
                    res = f_write(&file, temp, strlen(temp), &Bw);
                    if (res != FR_OK)
                    {
                        myPrintf("f_write error\r\n");
                    }
                }
                f_close(&file);

                u8g2_ClearDisplay(&u8g2);
                u8g2_SetFont(&u8g2, IR_Air32_font_12);
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "保存成功");
                u8g2_SendBuffer(&u8g2);
                HAL_Delay(1000);
                ResetState();
                MenuMode = MENU_UNIVERSAL_LEARN;
                MenuUniversalLearn();
                RunTime = HAL_GetTick();
            }
            break;

        case MENU_SEND:
            if (KeyState[1] == 1) // 按下�??
            {
                KeyState[1] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag >= 1)
                {
                    ArrowFlag = 0;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * 3, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                else
                {
                    ArrowFlag++;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag - 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[0] == 1) // 按上�??
            {
                KeyState[0] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag <= 0)
                {
                    // ArrowFlag = 3;
                    ArrowFlag = 0;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                else
                {
                    ArrowFlag--;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag + 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[2] == 1) // 确认
            {
                if (ArrowFlag == 0)
                {
                    ResetState();
                    MenuMode = MENU_SEND_SINGULAR;
                    MenuSendSingular(0);
                }
                else if (ArrowFlag == 1)
                {
                    ResetState();
                    MenuMode = MENU_SEND_PLURAL;
                    MenuSendPlural(0);
                }
            }
            else if (KeyState[3] == 1) // 取消
            {
                // KeyState[3] = 0;
                // MenuMode = MENU_BASIC;
                // ArrowFlag = 0;
                ResetState();
            }

            u8g2_SendBuffer(&u8g2);
            break;

        case MENU_SEND_SINGULAR:
            if (KeyState[1] == 1) // 按下�??
            {
                KeyState[1] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag >= 3)
                {
                    PageFlag++;
                    ArrowFlag = 0;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * 3, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                    MenuSendSingular(PageFlag);
                }
                else
                {
                    ArrowFlag++;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag - 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[0] == 1) // 按上�??
            {
                KeyState[0] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag <= 0)
                {
                    // ArrowFlag = 3;
                    ArrowFlag = 0;
                    if (PageFlag > 0)
                    {
                        PageFlag--;
                    }
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                    MenuSendSingular(PageFlag);
                }
                else
                {
                    ArrowFlag--;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag + 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[2] == 1) // 确认
            {
                KeyState[2] = 0;
                FIL file;
                int res;

                char filename[32];
                sprintf(filename, "%s", SD_FileName.data[PageFlag * 4 + ArrowFlag]);
                // sprintf(filename, "%d.IR", PageFlag * 4 + ArrowFlag);

                myPrintf("filename = %s\r\n", filename);
                res = f_open(&file, filename, FA_READ);
                if (res != FR_OK)
                {
                    myPrintf("f_open error\r\n");
                    if (res == FR_NO_FILE) // 文件不存在
                    {
                        myPrintf("No File.\r\n");
                        u8g2_ClearDisplay(&u8g2);
                        u8g2_SetFont(&u8g2, IR_Air32_font_12);
                        u8g2_DrawUTF8(&u8g2, (128 - 6 * 14) / 2, 12, "该数据未保存");
                        u8g2_SendBuffer(&u8g2);
                        HAL_Delay(1000);
                        ResetState();
                        MenuSend();
                        MenuMode = MENU_SEND;
                        break;
                    }
                    break;
                }
                char temp[2048];

                res = f_read(&file, temp, sizeof(temp), &Bw);
                if (res != FR_OK)
                {
                    myPrintf("f_write error\r\n");
                }
                if (strncmp(temp, "NEC", 3) == 0)
                {
                    myPrintf("NEC mode\r\n");
                    uint8_t NECCode[4];
                    sscanf(temp, "NEC%*c%*c0x%hhx 0x%hhx 0x%hhx 0x%hhx", &NECCode[0], &NECCode[1], &NECCode[2], &NECCode[3]);
                    myPrintf("NEC: 0x%hhx 0x%hhx 0x%hhx 0x%hhx\r\n", NECCode[0], NECCode[1], NECCode[2], NECCode[3]);
                    SendNEC(NECCode);
                }
                else if (strncmp(temp, "UNIVERSAL", 10) == 0)
                {
                    myPrintf("UNIVERSAL mode\r\n");
                    // 定义一个整数指针，用来指向动态分配的数组
                    uint16_t *a;
                    // 定义一个指针，用来指向分割后的字符串
                    char *token;
                    // 使用strtok函数以空格和换行符为分隔符，分割字符串
                    token = strtok(temp, " \r\n");

                    // 跳过第一个分割后的字符串"UNIVERSAL"
                    token = strtok(NULL, " \r\n");
                    // 跳过第二个分割后的字符串"Size"
                    token = strtok(NULL, " \r\n");
                    // 获取第三个分割后的字符串，即Size的值，并转换为整数
                    int size = atoi(token);
                    // 根据Size的值，动态分配数组的内存空间
                    a = malloc(size * sizeof(uint16_t));
                    // 定义一个变量，用来记录数组的索引
                    int i = 0;
                    // 循环读取剩余的分割后的字符串，并转换为整数，存储在数组中
                    while (token != NULL && i < size)
                    {
                        // 使用sscanf函数或atoi函数将字符串转换为整数
                        // sscanf(token, "%d", &a[i]);
                        a[i] = atoi(token);
                        // 增加数组的索引
                        i++;
                        // 获取下一个分割后的字符串
                        token = strtok(NULL, " \r\n");
                    }
                    // 打印数组中的元素
                    for (int j = 0; j < i; j++)
                    {
                        myPrintf("%d ", a[j]);
                    }
                    SendUniversal(a, size);
                    // 释放数组占用的内存空间
                    free(a);
                }
                else
                {
                    myPrintf("error\r\n");
                }

                f_close(&file);

                u8g2_ClearDisplay(&u8g2);
                u8g2_SetFont(&u8g2, IR_Air32_font_12);
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "发送成功");
                u8g2_SendBuffer(&u8g2);
                HAL_Delay(500);
                MenuSendSingular(0);
                ResetState();
                MenuMode = MENU_SEND_SINGULAR;
            }
            else if (KeyState[3] == 1) // 取消
            {
                // KeyState[3] = 0;
                // MenuMode = MENU_BASIC;
                // ArrowFlag = 0;
                // PageFlag = 0;
                ResetState();
                MenuSend();
                MenuMode = MENU_SEND;
            }

            u8g2_SendBuffer(&u8g2);
            break;

        case MENU_SEND_PLURAL:
        {                     // 加括号让编译器开心
            DIR dir;          // 目录对象
            FILINFO fileinfo; // 文件信息对象
            int res;
            res = f_opendir(&dir, "/"); // 打开目录
            if (res != FR_OK)
            {
                myPrintf("f_opendir error\r\n");
                break;
            }

            while (1)
            {
                if (KeyState[3] == 1) // 取消
                {
                    // KeyState[3] = 0;
                    // MenuMode = MENU_BASIC;
                    // ArrowFlag = 0;
                    // PageFlag = 0;
                    ResetState();
                    MenuMode = MENU_SEND;
                    break;
                }
                res = f_readdir(&dir, &fileinfo); // 读取目录项
                if (res != FR_OK || fileinfo.fname[0] == 0)
                {
                    myPrintf("All file data has been sent\r\n");
                    HAL_Delay(500);
                    f_closedir(&dir);           // 关闭目录
                    res = f_opendir(&dir, "/"); // 打开目录
                    if (res != FR_OK)
                    {
                        myPrintf("f_opendir error\r\n");
                        break;
                    }
                    // break; // 结束循环
                }
                if (fileinfo.fattrib & AM_DIR)
                { // 如果是子目录忽略不处理
                  // list_files(fileinfo.fname);
                }
                else
                {
                    myPrintf("%s\n", fileinfo.fname); // 如果是文件，则输出文件名

                    FIL file;
                    res = f_open(&file, fileinfo.fname, FA_READ);
                    if (res != FR_OK)
                    {
                        myPrintf("f_open error\r\n");
                        if (res == FR_NO_FILE) // 文件不存在
                        {
                            myPrintf("No File.\r\n");

                            ResetState();
                            MenuMode = MENU_SEND;
                        }
                    }
                    char temp[2048];

                    res = f_read(&file, temp, sizeof(temp), &Bw);
                    if (res != FR_OK)
                    {
                        myPrintf("f_read error\r\n");
                    }
                    if (strncmp(temp, "NEC", 3) == 0)
                    {
                        myPrintf("NEC mode\r\n");
                        uint8_t NECCode[4];
                        sscanf(temp, "NEC%*c%*c0x%hhx 0x%hhx 0x%hhx 0x%hhx", &NECCode[0], &NECCode[1], &NECCode[2], &NECCode[3]);
                        myPrintf("NEC: 0x%hhx 0x%hhx 0x%hhx 0x%hhx\r\n", NECCode[0], NECCode[1], NECCode[2], NECCode[3]);
                        SendNEC(NECCode);
                    }
                    else if (strncmp(temp, "UNIVERSAL", 10) == 0)
                    {
                        myPrintf("UNIVERSAL mode\r\n");
                        // 定义一个整数指针，用来指向动态分配的数组
                        uint16_t *a;
                        // 定义一个指针，用来指向分割后的字符串
                        char *token;
                        // 使用strtok函数以空格和换行符为分隔符，分割字符串
                        token = strtok(temp, " \r\n");

                        // 跳过第一个分割后的字符串"UNIVERSAL"
                        token = strtok(NULL, " \r\n");
                        // 跳过第二个分割后的字符串"Size"
                        token = strtok(NULL, " \r\n");
                        // 获取第三个分割后的字符串，即Size的值，并转换为整数
                        int size = atoi(token);
                        // 根据Size的值，动态分配数组的内存空间
                        a = malloc(size * sizeof(uint16_t));
                        // 定义一个变量，用来记录数组的索引
                        int i = 0;
                        // 循环读取剩余的分割后的字符串，并转换为整数，存储在数组中
                        while (token != NULL && i < size)
                        {
                            // 使用sscanf函数或atoi函数将字符串转换为整数
                            // sscanf(token, "%d", &a[i]);
                            a[i] = atoi(token);
                            // 增加数组的索引
                            i++;
                            // 获取下一个分割后的字符串
                            token = strtok(NULL, " \r\n");
                        }
                        // 打印数组中的元素
                        for (int j = 0; j < i; j++)
                        {
                            myPrintf("%d ", a[j]);
                        }
                        SendUniversal(a, size);
                        // 释放数组占用的内存空间
                        free(a);
                    }

                    f_close(&file);
                }
            }

            f_closedir(&dir); // 关闭目录

            ResetState();
            MenuMode = MENU_SEND_PLURAL;
            break;
        }

        case MENU_Record_View:
            if (KeyState[1] == 1) // 按下�??
            {
                KeyState[1] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag >= 1)
                {
                    ArrowFlag = 0;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * 3, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                else
                {
                    ArrowFlag++;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag - 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[0] == 1) // 按上�??
            {
                KeyState[0] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag <= 0)
                {
                    ArrowFlag = 3;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                else
                {
                    ArrowFlag--;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag + 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[2] == 1) // 确认
            {
                if (ArrowFlag == 0)
                {
                    ResetState();
                    MenuMode = MENU_Record_View_SINGULAR;
                    MenuRecordViewSingular(0);
                }
                else if (ArrowFlag == 1)
                {
                    ResetState();
                    MenuMode = MENU_Record_View_PLURAL;
                    MenuRecordViewPlural();
                }
            }
            else if (KeyState[3] == 1) // 取消
            {
                // KeyState[3] = 0;
                // MenuMode = MENU_BASIC;
                // ArrowFlag = 0;
                ResetState();
            }

            u8g2_SendBuffer(&u8g2);
            break;

        case MENU_Record_View_SINGULAR:
            if (KeyState[1] == 1) // 按下�??
            {
                KeyState[1] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag >= 3)
                {
                    PageFlag++;
                    ArrowFlag = 0;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * 3, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                    MenuRecordViewSingular(PageFlag);
                }
                else
                {
                    ArrowFlag++;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag - 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[0] == 1) // 按上�??
            {
                KeyState[0] = 0;

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);
                if (ArrowFlag <= 0)
                {
                    ArrowFlag = 3;
                    if (PageFlag > 0)
                    {
                        PageFlag--;
                    }
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12, 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                    MenuRecordViewSingular(PageFlag);
                }
                else
                {
                    ArrowFlag--;
                    u8g2_DrawGlyph(&u8g2, 0, 25 + 12 * ArrowFlag, 9654);
                    u8g2_SetDrawColor(&u8g2, 0);
                    u8g2_DrawBox(&u8g2, 0, 12 + 12 * (ArrowFlag + 1), 16, 16);
                    u8g2_SetDrawColor(&u8g2, 1);
                }
                myPrintf("ArrowFlag = %d\r\n", ArrowFlag);
            }
            else if (KeyState[2] == 1) // 确认
            {
                KeyState[2] = 0;
                FIL file;
                int res;

                u8g2_ClearDisplay(&u8g2);
                u8g2_SetFont(&u8g2, IR_Air32_font_12);
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 6) / 2, 12, "是否确认删除");
                u8g2_SendBuffer(&u8g2);

                while (KeyState[2] != 1 && KeyState[3] != 1)
                {
                    if (KeyState[2] == 1) // 确认
                    {
                        KeyState[2] = 0;
                        res = f_unlink((char *)SD_FileName.data[PageFlag * 4 + ArrowFlag]);
                        if (res != FR_OK)
                        {
                            myPrintf("delete file failed\r\n");
                            u8g2_ClearDisplay(&u8g2);
                            u8g2_SetFont(&u8g2, IR_Air32_font_12);
                            u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "删除失败");
                            u8g2_SendBuffer(&u8g2);
                            HAL_Delay(500);
                            MenuSendSingular(0);
                            ResetState();
                            MenuMode = MENU_Record_View_SINGULAR;
                            MenuRecordViewSingular(0);
                            break;
                        }

                        u8g2_ClearDisplay(&u8g2);
                        u8g2_SetFont(&u8g2, IR_Air32_font_12);
                        u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "删除成功");
                        u8g2_SendBuffer(&u8g2);
                        HAL_Delay(500);
                        MenuSendSingular(0);
                        ResetState();
                        MenuMode = MENU_Record_View_SINGULAR;
                        MenuRecordViewSingular(0);
                        break;
                    }
                    else if (KeyState[3] == 1) // 取消
                    {
                        KeyState[3] = 0;
                        MenuSendSingular(0);
                        ResetState();
                        MenuMode = MENU_Record_View_SINGULAR;
                        MenuRecordViewSingular(0);
                        break;
                    }
                }
            }
            else if (KeyState[3] == 1) // 取消
            {
                // KeyState[3] = 0;
                // MenuMode = MENU_BASIC;
                // ArrowFlag = 0;
                // PageFlag = 0;
                ResetState();
            }

            u8g2_SendBuffer(&u8g2);
            break;

        case MENU_Record_View_PLURAL:
            if (KeyState[2] == 1) // 确认
            {
                FIL file;
                int res;
                KeyState[2] = 0;
                for (size_t i = 0; i < SD_FileName.size; i++)
                {
                    res = f_unlink(SD_FileName.data[i]);
                    if (res != FR_OK)
                    {
                        myPrintf("delete file failed\r\n");
                        u8g2_ClearDisplay(&u8g2);
                        u8g2_SetFont(&u8g2, IR_Air32_font_12);
                        u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "删除失败");
                        u8g2_SendBuffer(&u8g2);
                        HAL_Delay(500);
                        ResetState();
                        MenuMode = MENU_Record_View;
                        ArrowFlag = 0;
                        MenuRecordView();
                        break;
                    }
                }

                u8g2_ClearDisplay(&u8g2);
                u8g2_SetFont(&u8g2, IR_Air32_font_12);
                u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "删除成功");
                u8g2_SendBuffer(&u8g2);
                HAL_Delay(500);
                ResetState();
                MenuMode = MENU_Record_View;
                ArrowFlag = 0;
                MenuRecordView();
                break;
            }
            else if (KeyState[3] == 1) // 取消
            {
                KeyState[3] = 0;
                MenuSendSingular(0);
                ResetState();
                MenuMode = MENU_Record_View;
                ArrowFlag = 0;
                MenuRecordView();
                break;
            }
            break;

        default:
            break;
        }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief NVIC Configuration.
 * @retval None
 */
static void MX_NVIC_Init(void)
{
    /* TIM4_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

/* USER CODE BEGIN 4 */
void myPrintf(const char *fmt, ...)
{
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 0xFFFF);
}

/**
 * @brief 输入捕获回调函数
 *
 * @param htim
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        IRCaptureCallback();
    }
}

/**
 * @brief 定时器溢出回调函�???????????
 *
 * @param htim
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        IRPeriodElapsedCallback();
    }
    else if (htim->Instance == TIM3)
    {
        ScanKeyboard();
    }
}

uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {

    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;

    case U8X8_MSG_DELAY_MILLI:
        HAL_Delay(arg_int);
        break;

    case U8X8_MSG_GPIO_I2C_CLOCK:
        break;

    case U8X8_MSG_GPIO_I2C_DATA:
        break;

    default:
        return 0;
    }
    return 1; // command processed successfully.
}

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[32]; /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg)
    {

    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0)
        {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }
        break;

    case U8X8_MSG_BYTE_INIT:
        /* add your custom code to init i2c subsystem */
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;

    case U8X8_MSG_BYTE_END_TRANSFER:
        HAL_I2C_Master_Transmit(&hi2c1, u8x8_GetI2CAddress(u8x8), buffer, buf_idx, 1000);
        break;

    default:
        return 0;
    }
    return 1;
}

void ShowStartScreen(void)
{
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_16);
    u8g2_DrawUTF8(&u8g2, (128 - 16 * 4) / 2, 12, "遥控管家");

    u8g2_SetFont(&u8g2, u8g2_font_wqy16_t_chinese1);
    u8g2_DrawUTF8(&u8g2, (128 - 6 * 11) / 2, 28, "Loading...");

    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 12 * 9) / 2, 40, "若长时间未加载完成");
    u8g2_DrawUTF8(&u8g2, (128 - 12 * 11) / 2, 52, "请检查SD卡是否正确插入");
    u8g2_SendBuffer(&u8g2);
}

/**
 * @brief 扫描按键�????1ms进入�????次中断，如果有按下，那么5ms之后再次�????查，如果还是按下，那么就认为是按下了，否则就是抖动，并把状�?�存入按键缓冲区
 *
 */
void ScanKeyboard(void)
{
    static uint8_t keyCount[4] = {0};

    GPIO_TypeDef *keyPort[4] = {UP_GPIO_Port, DOWN_GPIO_Port, FORWARD_GPIO_Port, BACK_GPIO_Port};
    uint16_t keyPin[4] = {UP_Pin, DOWN_Pin, FORWARD_Pin, BACK_Pin};

    for (size_t i = 0; i < 4; i++)
    {
        if (HAL_GPIO_ReadPin(keyPort[i], keyPin[i]) == GPIO_PIN_SET) // 高电平有�????
        {
            keyCount[i]++;
            if (keyCount[i] > 5)
            {
                keyCount[i] = 0;
                KeyState[i] = 1;

                HAL_TIM_Base_Stop_IT(&htim3);
                while (HAL_GPIO_ReadPin(keyPort[i], keyPin[i]) == GPIO_PIN_SET)
                    ;
                HAL_TIM_Base_Start_IT(&htim3);
            }
        }
        else
        {
            keyCount[i] = 0;
        }
    }
}

void ResetKeyboard(void)
{
    for (size_t i = 0; i < 4; i++)
    {
        KeyState[i] = 0;
    }
}

void ResetState(void)
{
    ResetKeyboard();
    ArrowFlag = 0;
    MenuMode = MENU_BASIC;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: myPrintf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
