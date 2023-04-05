#ifndef MENU_H__
#define MENU_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx_hal.h"
#include "u8g2.h"

    void MenuBasic(void);
    void MenuNECLearn(void);
    void MenuSaveNEC(uint16_t page);
    void MenuUniversalLearn(void);
    void MenuSaveUniversal(uint16_t page);
    void MenuSend(void);
    void MenuSendSingular(uint16_t page);
    void MenuSendPlural(uint16_t page);

    void MenuRecordView();
    void MenuRecordViewSingular(uint16_t page);
    void MenuRecordViewPlural(void);

    /**
     * @brief 生成随机字符串作为文件名
     * 
     * @param buff 
     * @param length 
     */
    void genRandomString(char* buff, int length);

    typedef enum
    {
        MENU_BASIC,           // 基本菜单
        MENU_BASIC_SELECT,    // 基本菜单选择
        MENU_NEC_LEARN,       // NEC学习
        MENU_SAVE_NEC,        // 保存NEC码
        MENU_UNIVERSAL_LEARN, // 万能学习
        MENU_SAVE_UNIVERSAL,  // 保存万能学习

        MENU_SEND,          // 发送
        MENU_SEND_SINGULAR, // 发送单个数据
        MENU_SEND_PLURAL,   // 发送多个数据

        MENU_Record_View,          // 记录查看
        MENU_Record_View_SINGULAR, // 记录单个删除
        MENU_Record_View_PLURAL,   // 记录多个删除

    } Menu_mode_t;

    typedef struct 
    {
        uint16_t size;
        char data[128][12];
    }SD_FileName_t;
    

    extern Menu_mode_t MenuMode;
    extern u8g2_t u8g2;
    extern SD_FileName_t SD_FileName;
#ifdef __cplusplus
}
#endif
#endif