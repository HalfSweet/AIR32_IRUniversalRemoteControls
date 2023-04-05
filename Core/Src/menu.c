#include "menu.h"
#include <stdlib.h>
#include <stdio.h>
#include "ff.h"
#include <string.h>
Menu_mode_t MenuMode = MENU_BASIC;
SD_FileName_t SD_FileName;

void MenuBasic(void)
{
    const char *mode[] = {"普通学习", "万能学习", "发送模式", "记录查看"};
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_16);
    u8g2_DrawUTF8(&u8g2, (128 - 16 * 2) / 2, 12, "菜单");

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols); // 画箭头初始位置
    u8g2_DrawGlyph(&u8g2, 0, 25, 9654);

    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    for (size_t i = 0; i < 4; i++)
    {
        u8g2_DrawUTF8(&u8g2, 16, 25 + 12 * i, mode[i]);
    }

    u8g2_SendBuffer(&u8g2);
}

void MenuNECLearn(void)
{
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 16 * 6) / 2, 12, "普通学习中......");
    // u8g2_DrawUTF8(&u8g2, (128 - 6 * 11) / 2, 24, "Waiting...");
    u8g2_SendBuffer(&u8g2);
}

void MenuSaveNEC(uint16_t page)
{
    // 逻辑有变，这个函数没用啦
    // const char *mode[] = {"0", "Electric Fan", "Television", "DVD"};
    // u8g2_ClearDisplay(&u8g2);
    // u8g2_SetFont(&u8g2, IR_Air32_font_12);
    // u8g2_DrawUTF8(&u8g2, (128 - 12 * 6) / 2, 12, "保存中......");

    // u8g2_SendBuffer(&u8g2);
}

void MenuUniversalLearn(void)
{
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 16 * 6) / 2, 12, "万能学习中......");
    // u8g2_DrawUTF8(&u8g2, (128 - 6 * 11) / 2, 24, "Waiting...");
    u8g2_SendBuffer(&u8g2);
}

void MenuSaveUniversal(uint16_t page)
{
    // const char *mode[] = {"0", "Electric Fan", "Television", "DVD"};
    // u8g2_ClearDisplay(&u8g2);
    // u8g2_SetFont(&u8g2, IR_Air32_font_12);
    // u8g2_DrawUTF8(&u8g2, (128 - 12 * 3) / 2, 12, "请保存");

    // u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols); // 画箭头初始位置
    // u8g2_DrawGlyph(&u8g2, 0, 25, 9654);

    // u8g2_SetFont(&u8g2, IR_Air32_font_12);
    // char temp[4];
    // for (size_t i = 0; i < 4; i++)
    // {
    //     sprintf(temp, "%d", page * 4 + i);
    //     u8g2_DrawUTF8(&u8g2, 16, 25 + 12 * i, temp);
    // }

    // u8g2_SendBuffer(&u8g2);
}

void MenuSend(void)
{
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "发送模式");

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols); // 画箭头初始位置
    u8g2_DrawGlyph(&u8g2, 0, 25, 9654);

    char *mode[] = {"单个发送", "连续发送"};

    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    for (size_t i = 0; i < 2; i++)
    {
        u8g2_DrawUTF8(&u8g2, 16, 25 + 12 * i, mode[i]);
    }

    u8g2_SendBuffer(&u8g2);
}

void MenuSendSingular(uint16_t page)
{
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 12 * 9) / 2, 12, "请选择要发送的数据");

    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    if (page == 0)
    {
        DIR dir;          // 目录对象
        FILINFO fileinfo; // 文件信息对象
        int res;
        res = f_opendir(&dir, "/"); // 打开目录
        if (res != FR_OK)
        {
            myPrintf("f_opendir error\r\n");
            return;
        }
        SD_FileName.size = 0;

        while (1)
        {
            res = f_readdir(&dir, &fileinfo); // 读取目录项
            if (res != FR_OK || fileinfo.fname[0] == 0)
            {
                myPrintf("All file data has been sent\r\n");
                ;
                break; // 结束循环
            }
            if (fileinfo.fattrib & AM_DIR)
            { // 如果是子目录忽略不处理
              // list_files(fileinfo.fname);
            }
            else
            {
                myPrintf("%s\n", fileinfo.fname); // 如果是文件，则输出文件名
                SD_FileName.size++;
                strcpy(SD_FileName.data[SD_FileName.size - 1], fileinfo.fname);
            }
        }
        f_closedir(&dir); // 关闭目录
    }

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols); // 画箭头初始位置
    u8g2_DrawGlyph(&u8g2, 0, 25, 9654);

    u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_chinese3);
    char temp[16];
    for (size_t i = 0; i < 4; i++)
    {
        if (SD_FileName.size < page * 4 + i)
        {
            break;
        }
        sprintf(temp, "%s", SD_FileName.data[page * 4 + i]);
        u8g2_DrawUTF8(&u8g2, 16, 25 + 12 * i, temp);
    }

    u8g2_SendBuffer(&u8g2);
}

void MenuSendPlural(uint16_t page)
{
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 12 * 3) / 2, 12, "发送中");

    u8g2_SendBuffer(&u8g2);
}

void MenuRecordView()
{
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 12 * 4) / 2, 12, "记录管理");

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols); // 画箭头初始位置
    u8g2_DrawGlyph(&u8g2, 0, 25, 9654);

    char *mode[] = {"单个删除", "全部删除"};

    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    for (size_t i = 0; i < 2; i++)
    {
        u8g2_DrawUTF8(&u8g2, 16, 25 + 12 * i, mode[i]);
    }

    u8g2_SendBuffer(&u8g2);
}

void MenuRecordViewSingular(uint16_t page)
{

    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 12 * 8) / 2, 12, "选择要删除的文件");

    if (page == 0)
    {
        DIR dir;          // 目录对象
        FILINFO fileinfo; // 文件信息对象
        int res;
        res = f_opendir(&dir, "0:/"); // 打开目录
        if (res != FR_OK)
        {
            myPrintf("f_opendir error\r\n");
            return;
        }
        SD_FileName.size = 0;

        while (1)
        {
            // if (KeyState[3] == 1) // 取消
            // {
            //     // KeyState[3] = 0;
            //     // MenuMode = MENU_BASIC;
            //     // ArrowFlag = 0;
            //     // PageFlag = 0;
            //     ResetState();
            //     MenuMode = MENU_SEND;
            //     break;
            // }
            res = f_readdir(&dir, &fileinfo); // 读取目录项
            if (res != FR_OK || fileinfo.fname[0] == 0)
            {
                myPrintf("All file data has been sent\r\n");
                break; // 结束循环
            }
            if (fileinfo.fattrib & AM_DIR)
            { // 如果是子目录忽略不处理
              // list_files(fileinfo.fname);
            }
            else
            {
                myPrintf("filename is %s\n", fileinfo.fname); // 如果是文件，则输出文件名
                SD_FileName.size++;
                strcpy(SD_FileName.data[SD_FileName.size - 1], fileinfo.fname);
            }
        }
        f_closedir(&dir); // 关闭目录
    }
    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols); // 画箭头初始位置
    u8g2_DrawGlyph(&u8g2, 0, 25, 9654);

    u8g2_SetFont(&u8g2, u8g2_font_wqy12_t_chinese3);
    char temp[16];
    for (size_t i = 0; i < 4; i++)
    {
        if (SD_FileName.size < page * 4 + i)
        {
            break;
        }
        //sprintf(temp, "%s", SD_FileName.data[page * 4 + i]);
        myPrintf("filename is :%s\r\n", SD_FileName.data[page * 4 + i]);
        u8g2_DrawUTF8(&u8g2, 16, 25 + 12 * i, SD_FileName.data[page * 4 + i]);
    }

    u8g2_SendBuffer(&u8g2);
}

void MenuRecordViewPlural(void)
{
    u8g2_ClearDisplay(&u8g2);
    u8g2_SetFont(&u8g2, IR_Air32_font_12);
    u8g2_DrawUTF8(&u8g2, (128 - 12 * 8) / 2, 12, "确认是否全部删除");
    DIR dir;          // 目录对象
    FILINFO fileinfo; // 文件信息对象
    int res;
    res = f_opendir(&dir, "/"); // 打开目录
    if (res != FR_OK)
    {
        myPrintf("f_opendir error\r\n");
        return;
    }
    SD_FileName.size = 0;

    while (1)
    {
        // if (KeyState[3] == 1) // 取消
        // {
        //     // KeyState[3] = 0;
        //     // MenuMode = MENU_BASIC;
        //     // ArrowFlag = 0;
        //     // PageFlag = 0;
        //     ResetState();
        //     MenuMode = MENU_SEND;
        //     break;
        // }
        res = f_readdir(&dir, &fileinfo); // 读取目录项
        if (res != FR_OK || fileinfo.fname[0] == 0)
        {
            myPrintf("All file data has been sent\r\n");
            break; // 结束循环
        }
        if (fileinfo.fattrib & AM_DIR)
        { // 如果是子目录忽略不处理
          // list_files(fileinfo.fname);
        }
        else
        {
            myPrintf("%s\n", fileinfo.fname); // 如果是文件，则输出文件名
            SD_FileName.size++;
            strcpy(SD_FileName.data[SD_FileName.size - 1], fileinfo.fname);
        }
    }
    f_closedir(&dir); // 关闭目录

    u8g2_SendBuffer(&u8g2);
}

void genRandomString(char *buff, int length)
{
    char metachar[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    int i = 0;
    srand(HAL_GetTick()); // 用时间做种，每次产生随机数不一样
    for (i = 0; i < length - 1; i++)
    {
        buff[i] = metachar[rand() % 36];
    }
    buff[length - 1] = '\0';
}