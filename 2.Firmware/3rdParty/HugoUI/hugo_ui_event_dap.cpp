#include "hugo_ui_event_dap.h"
#include "hugo_ui_widget.h"
#include "SWD_flash.h"
#include <cstring>
#include <cstdlib>
#include <stdint.h>

// Fatfs
#include "ff.h"

// CMSIS-DAP
#include "DAP.h"
#include "SWD_host.h"
#include "SWD_flash.h"

// test
#include "interface_uart.h"



using namespace HugoUI;

/* 全局变量 ----------------------------------------------------------- */

static char firmwareName[256] = "";

// 开关控件变量 / 最大支持256个固件文件
static bool firmwareFlag[256] = {false};
// flash算法选择控件变量 / 最大支持64个算法
static bool flashAlgoFlag[64] = {false};
// 局部扇区擦除标志变量 / true: 局部扇区擦除, false: 全片擦除
bool flashEraseSectorFlag = false;

// flash起始地址变量
static uint32_t mcuFlashAddress = 0x8000000;
// 扇区大小 (默认1KB, 根据不同MCU调整)
static uint32_t flashSectorSize = 1024;

// 算法列表
// 算法对象来自「CMSIS-DAP」目录
extern const program_target_t flash_algo_STM32F10x;
extern const program_target_t flash_algo_STM32F4xx;
extern const program_target_t flash_algo_STM32F0xx;
extern const program_target_t flash_algo_GD32F30x;

typedef struct flash_algo_info_t
{
    const char *name;
    const program_target_t *algo;
} flash_algo_info_t;

static const flash_algo_info_t flashAlgoList[] = {
    {"STM32F10x", &flash_algo_STM32F10x},
    {"STM32F4xx", &flash_algo_STM32F4xx},
    {"STM32F0xx", &flash_algo_STM32F0xx},
    {"GD32F30x", &flash_algo_GD32F30x},
};

static const int flashAlgoCount = sizeof(flashAlgoList) / sizeof(flash_algo_info_t);

/* 用户函数 ----------------------------------------------------------- */
bool convertHexToBin(const char *hexPath, const char *binPath);

// CMSIS-DAP 页面 批量添加算法
void HugoUI::AddItemsFromFlashAlgo(Page::Ptr page)
{
    if (page == nullptr)
        return;

    // 遍历所有算法
    for (int i = 0; i < flashAlgoCount; i++)
    {
        if (i < 64) // 确保不超过 flashAlgoFlag 数组大小
        {
            page->AddItem(flashAlgoList[i].name, ItemType::Checkbox, &flashAlgoFlag[i], EventSelectFlashAlgo);
        }
    }

    // 默认选择STM32F4xx算法
    swd_flash_select_algo((const program_target_t *)&flash_algo_STM32F4xx);
    flashAlgoFlag[1] = true;
}

// CMSIS-DAP 页面 批量AddItem
void HugoUI::AddItemsFromFirmwareFolder(Page::Ptr page, const char *folderPath)
{
    if (page == nullptr || folderPath == nullptr)
        return;

    DIR dir;
    FILINFO fno;
    FRESULT res;

    // 打开文件夹
    res = f_opendir(&dir, (const TCHAR *)folderPath);
    if (res != FR_OK)
    {
        page->AddItem("-> Firmware <-", ItemType::Description);
        page->AddItem("该文件夹不存在", ItemType::Description);
        return;
    }

    // 遍历所有文件
    for (;;)
    {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0)
            break;

        // 跳过文件夹，只添加文件
        if (fno.fattrib & AM_DIR)
            continue;

        // 添加文件名作为 Item，点击触发回调
        page->AddItem((char *)fno.fname, ItemType::Checkbox, &firmwareFlag[page->itemMax], EventSelectFirmware);
    }
    f_closedir(&dir);

    // 默认选择第一个文件
    snprintf(firmwareName, sizeof(firmwareName), "0:/Firmware/%s", page->items[1]->title.c_str());
    firmwareFlag[1] = true;

    Usart_debugMsg("[AddItem-DAP] firmwareName:%s, firmwareFlag[0]:%d", firmwareName, firmwareFlag[0]);
}

// CMSIS-DAP 烧录 的应用事件函数
void HugoUI::EventBurnDapUI(void)
{
    static uint8_t isTestDapInit = 0;
    static uint8_t isEnterAnimFinish = 0;
    static int8_t isWriteFinish = 0;

    // 所有变量都放在函数内部
    FRESULT res;
    FIL fnew;
    uint8_t rData[1024];
    uint8_t checkData[1024];
    uint8_t readFlag;
    uint32_t burnCount = 0;
    char buf[16]; // 专门用来显示数字

    // Init
    if (!isTestDapInit)
    {
        isTestDapInit = 1;
    }
    else
    {
        oled_draw_str(90, FONT_HEIGHT * 4, "Init OK");
    }

    // Enter Anim
    if (!isEnterAnimFinish)
    {
        oled_draw_box(0, 0,  SCREEN_WIDTH, SCREEN_HEIGHT);
        if (Transition_Blur() == 0)
            isEnterAnimFinish = 1;
        oled_send_buffer();
        return;
    }

    // Loop
    oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP烧录』");

    if (isWriteFinish == 1)
    {
        oled_draw_UTF8(0, FONT_HEIGHT * 2, "烧录非常的成功!!!");
        oled_draw_UTF8(0, FONT_HEIGHT * 3, "<<长按编码器退出:)");
    }
    else if (isWriteFinish == -1)
    {
        oled_draw_UTF8(0, FONT_HEIGHT * 2, "失败,检查算法和文件!");
        oled_draw_UTF8(0, FONT_HEIGHT * 3, "<<长按编码器退出:)");
    }
    else
    {
        if (swd_init_debug())
        {
            oled_clear_buffer();
            oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP烧录』");
            oled_draw_UTF8(0, FONT_HEIGHT * 2, "目标芯片已连接!!!");
            oled_send_buffer();

            if (target_flash_init(mcuFlashAddress) == ERROR_SUCCESS)
            {
                // 检查是否为HEX文件并转换（先检查文件获取大小）
                const char* ext = strrchr(firmwareName, '.');
                if (ext && strcmp(ext, ".hex") == 0) {
                    char binName[256];
                    strcpy(binName, firmwareName);
                    strcpy(strrchr(binName, '.'), ".bin");

                    FIL testFile;
                    if (f_open(&testFile, (const TCHAR*)binName, FA_READ) == FR_OK) {
                        f_close(&testFile);
                        // 同名BIN存在，使用BIN
                        strcpy(firmwareName, binName);
                    } else {
                        // 不存在，转换HEX到BIN
                        oled_clear_buffer();
                        oled_draw_UTF8(0, FONT_HEIGHT, "『转换HEX到BIN』");
                        oled_draw_UTF8(0, FONT_HEIGHT * 2, "正在转换...");
                        oled_send_buffer();

                        if (convertHexToBin(firmwareName, binName)) {
                            // 转换成功，使用BIN
                            strcpy(firmwareName, binName);
                        } else {
                            // 转换失败
                            oled_clear_buffer();
                            oled_draw_UTF8(0, FONT_HEIGHT, "『转换失败』");
                            oled_draw_UTF8(0, FONT_HEIGHT * 2, "检查HEX文件!");
                            oled_send_buffer();
                            HAL_Delay(2000);
                            isWriteFinish = -1;
                            return;
                        }
                    }
                }

                // 获取固件文件大小
                uint32_t firmwareSize = 0;
                res = f_open(&fnew, (const TCHAR *)firmwareName, FA_READ);
                if (res == FR_OK)
                {
                    firmwareSize = f_size(&fnew);
                    f_close(&fnew);
                }

                // 擦除操作：根据flashEraseSectorFlag选择擦除方式
                bool eraseSuccess = false;
                if (flashEraseSectorFlag)
                {
                    // 局部扇区擦除模式
                    oled_clear_buffer();
                    oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP烧录』");
                    oled_draw_UTF8(0, FONT_HEIGHT * 2, "局部扇区擦除中...");
                    oled_send_buffer();

                    // 计算需要擦除的扇区数量
                    uint32_t sectorCount = (firmwareSize + flashSectorSize - 1) / flashSectorSize;
                    uint32_t eraseAddr = 0x08000000;

                    for (uint32_t i = 0; i < sectorCount; i++)
                    {
                        if (target_flash_erase_sector(eraseAddr) != ERROR_SUCCESS)
                        {
                            oled_clear_buffer();
                            oled_draw_UTF8(0, FONT_HEIGHT, "『扇区擦除失败』");
                            sprintf(buf, "扇区:%d", i);
                            oled_draw_str(0, FONT_HEIGHT * 2, buf);
                            oled_send_buffer();
                            HAL_Delay(1000);
                            isWriteFinish = -1;
                            return;
                        }
                        eraseAddr += flashSectorSize;

                        // 显示擦除进度
                        oled_clear_buffer();
                        oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP烧录』");
                        oled_draw_UTF8(0, FONT_HEIGHT * 2, "局部扇区擦除中...");
                        sprintf(buf, "%d/%d", (int)(i + 1), (int)sectorCount);
                        oled_draw_str(0, FONT_HEIGHT * 3, buf);
                        oled_send_buffer();
                    }
                    eraseSuccess = true;
                }
                else
                {
                    // 全片擦除模式
                    oled_clear_buffer();
                    oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP烧录』");
                    oled_draw_UTF8(0, FONT_HEIGHT * 2, "正在擦除目标芯片!!!");
                    oled_send_buffer();

                    if (target_flash_erase_chip() == ERROR_SUCCESS)
                    {
                        eraseSuccess = true;
                    }
                }

                if (eraseSuccess)
                {
                    oled_clear_buffer();
                    oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP烧录』");
                    oled_draw_UTF8(0, FONT_HEIGHT * 2, flashEraseSectorFlag ? "扇区已擦除!!!" : "目标芯片已擦除!!!");
                    oled_send_buffer();

                    // 烧录核心内容 begin ------------------------------------------
                    res = f_open(&fnew, (const TCHAR *)firmwareName, FA_READ);
                    if (res == FR_OK)
                    {
                        uint32_t progress = 0, burnAddr = 0;
                        UINT bytesRead;
                        readFlag = 1;

                        while (readFlag)
                        {
                            f_read(&fnew, rData, 1024, &bytesRead);
                            if (bytesRead < 1024)
                                readFlag = 0;

                            if (target_flash_program_page(0x08000000 + burnAddr, rData, 1024) == ERROR_SUCCESS)
                            {
                                swd_read_memory(0x08000000 + burnAddr, checkData, 1024);
                                if (memcmp(checkData, rData, 1024) != 0)
                                {
                                    oled_clear_buffer();
                                    oled_draw_UTF8(20, FONT_HEIGHT * 1, "校验失败!!!");
                                    oled_send_buffer();
                                    HAL_Delay(1000);
                                    burnAddr = 0;
                                    readFlag = 1;
                                    f_close(&fnew);
                                    return;
                                }

                                burnAddr += 1024;
                                progress = ((double)burnAddr / f_size(&fnew)) * 100;

                                // 安全显示进度（无编译错误）
                                oled_clear_buffer();
                                oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP烧录』");
                                oled_draw_UTF8(0, FONT_HEIGHT * 2, "烧录中");
                                oled_draw_UTF8(0, FONT_HEIGHT * 3, "进度:");
                                sprintf(buf, "%d%%", (int)progress);
                                oled_draw_str(50, FONT_HEIGHT * 3, buf);
                                oled_send_buffer();
                            }
                            else
                            {
                                f_close(&fnew);
                                return;
                            }
                        }
                        burnCount++;
                        oled_clear_buffer();
                        oled_draw_UTF8(0, FONT_HEIGHT, "『CMSIS-DAP烧录』");
                        oled_draw_UTF8(0, FONT_HEIGHT * 2, "烧录完成!!!");
                        oled_send_buffer();
                        isWriteFinish = 1;
                        swd_set_target_reset(0);
                        HAL_Delay(1000);
                        f_close(&fnew);
                    }
                    // 烧录核心内容 end --------------------------------------------
                }
            }
            else
            {
                isWriteFinish = -1;
            }
        }
        else
        {
            oled_draw_UTF8(0, FONT_HEIGHT * 2, "还没有芯片接入噢...");
            oled_send_area_buffer(0, FONT_HEIGHT * 2, SCREEN_WIDTH, FONT_HEIGHT);
        }
    }

    // Exit
    if (uiKeyNumInSide == 2)
    {
        uint8_t isExitAnimFinish = 0;
        oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        oled_send_buffer();

        while (!isExitAnimFinish)
        {
            if (Transition_Blur() == 0)
                isExitAnimFinish = 1;
            oled_send_buffer();
        }

        isEnterAnimFinish = 0;
        isWriteFinish = 0;
    }
}

// 设置Flash起始地址的应用事件函数
void HugoUI::EventSetFlashAddressUI(void)
{
    static uint8_t isInit = 0;
    static uint8_t isEnterAnimFinish = 0;
    static int8_t selectIdx = 0;
    static bool isSelect = false;
    static uint8_t digits[7];

    // Init
    if (!isInit)
    {
        isInit = 1;
        uint32_t addr = mcuFlashAddress;
        for(int i = 0; i < 7; i++){
            digits[i] = (addr >> (24 - i * 4)) & 0xF;
        }
    }
    else
    {
        oled_draw_str(90, FONT_HEIGHT * 4, "Init OK");
    }

    // Enter Anim
    if (!isEnterAnimFinish)
    {
        oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (Transition_Blur() == 0)
            isEnterAnimFinish = 1;
        oled_send_buffer();
        return;
    }

    // Loop
    oled_draw_UTF8(0, FONT_HEIGHT, "『设置Flash起始地址』");
    oled_draw_UTF8(0, FONT_HEIGHT * 2 + 1, ">");
    oled_set_font(u8g2_font_VCR_OSD_mu);
    char addrStr[20];
    sprintf(addrStr, "0X%X%X%X%X%X%X%X", digits[0], digits[1], digits[2], digits[3], digits[4], digits[5], digits[6]);
    oled_draw_UTF8(12, FONT_HEIGHT * 2 + 5, addrStr);
    oled_set_font(u8g2_font_maniac_tr);
    char digitStr[2];
    sprintf(digitStr, "%X", digits[selectIdx]);
    oled_draw_UTF8(12, FONT_HEIGHT * 4 + 5, digitStr);
    oled_set_font(u8g2_font_wqy13_t_gb2312a);

    oled_set_draw_color(2);
    // 以500ms为频率进行闪烁，选中时
    if (!isSelect || ((get_ticks() / 500) % 2 == 0)) {
        oled_draw_box(8, 36, 25, 28);
    }
    oled_draw_box(36 + selectIdx * 12, 17, 12, 16);
    oled_set_draw_color(1);

    // Ctrl
    if (!isSelect)
    {
        if (uiEncoderNumInSide == 1)
        {
            selectIdx = selectIdx >= 6 ? 6 : selectIdx + 1;
        }
        else if (uiEncoderNumInSide == 2)
        {
            selectIdx = selectIdx <= 0 ? 0 : selectIdx - 1;
        }
    }
    else
    {
        if (uiEncoderNumInSide == 1)
        {
            digits[selectIdx] = (digits[selectIdx] + 1) % 16;
        }
        else if (uiEncoderNumInSide == 2)
        {
            digits[selectIdx] = (digits[selectIdx] - 1 + 16) % 16;
        }
        // 更新 mcuFlashAddress
        mcuFlashAddress = 0;
        for(int i = 0; i < 7; i++){
            mcuFlashAddress |= (uint32_t)digits[i] << (24 - i * 4);
        }
    }

    if(uiKeyNumInSide == 1)
    {
        isSelect = !isSelect;
    }
    else if (uiKeyNumInSide == 2)
    {
        // Exit
        uint8_t isExitAnimFinish = 0;
        oled_draw_box(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        oled_send_buffer();

        while (!isExitAnimFinish)
        {
            if (Transition_Blur() == 0)
                isExitAnimFinish = 1;
            oled_send_buffer();
        }
        isSelect = 0;
        isEnterAnimFinish = 0;
    }
}

void HugoUI::EventEraseChipUI(void)
{

    WidgetDrawMessageBox("擦除芯片...");

    // Util
    if (target_flash_init(0x08000000) == ERROR_SUCCESS)
    {
        oled_send_buffer();
        if (target_flash_erase_chip() == ERROR_SUCCESS)
        {
            uiKeyNumInSide = 2; // 自动退出
        }
    }
    else
    {
        WidgetDrawMessageBox("连接失败!", true);
    }

    // Exit
    if (uiKeyNumInSide == 2)
    {
    }
}

void HugoUI::EventEraseSectorInfoBar(void)
{
    // Show Widget
    if (flashEraseSectorFlag)
        WidgetPushInfoBar("局部擦除较慢!", 2000);
    else
        WidgetPushInfoBar("全片擦除较快!", 2000);
}

void HugoUI::EventAutoTriggerUI(void)
{

    WidgetDrawMessageBox("文本测试无功能...", true);

    // Util
    delay(3000);
    uiKeyNumInSide = 2; // 自动退出

    // Exit
    if (uiKeyNumInSide == 2)
    {
    }
}

void HugoUI::EventSelectFirmware(void)
{
    // 1. 清空之前所有选择状态
    memset(firmwareFlag, 0, sizeof(firmwareFlag));

    // 2. 当前选中的 firmware item 置 true
    if (currentItem && currentItem->flag)
    {
        *currentItem->flag = true;
    }

    // 3. 更新要烧录的固件路径（如果当前Item有名字）
    if (currentItem && !currentItem->title.empty())
    {
        // 固件文件夹固定为 0:/Firmware/
        snprintf(firmwareName, sizeof(firmwareName), "0:/Firmware/%s", currentItem->title.c_str());
    }
}

// 统一的算法选择回调
void HugoUI::EventSelectFlashAlgo(void)
{
    // 检查 currentItem 是否有效
    if (!currentItem || currentItem->title.empty())
        return;

    // 遍历算法列表
    for (int i = 0; i < flashAlgoCount; i++)
    {
        if (strcmp(currentItem->title.c_str(), flashAlgoList[i].name) == 0)
        {
            // 清空之前所有算法 flag
            memset(flashAlgoFlag, 0, sizeof(flashAlgoFlag));

            // 设置当前选中的 flag
            if (currentItem->flag)
                *currentItem->flag = true;

            swd_flash_select_algo(flashAlgoList[i].algo);
            return;
        }
    }
}

static uint8_t HexNibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

static uint8_t ParseHexByte(const char* p)
{
    return (HexNibble(p[0]) << 4) | HexNibble(p[1]);
}

// Hex to Bin 转换函数
bool convertHexToBin(const char* hexPath, const char* binPath)
{
    FIL hexFile, binFile;
    FRESULT res;

    // 打开hex文件
    res = f_open(&hexFile, (const TCHAR*)hexPath, FA_READ);
    if (res != FR_OK) return false;

    // 打开或创建bin文件
    res = f_open(&binFile, (const TCHAR*)binPath, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        f_close(&hexFile);
        return false;
    }

    char line[256];
    uint32_t baseAddr = 0;
    uint32_t currentPos = 0;
    const uint32_t flashBase = 0x08000000;

    // 读取hex文件行
    while (f_gets((TCHAR*)line, sizeof(line), &hexFile)) {
        if (line[0] != ':')
            continue;

        uint8_t len = ParseHexByte(line + 1);
        uint16_t addr = (uint16_t)((ParseHexByte(line + 3) << 8) | ParseHexByte(line + 5));
        uint8_t type = ParseHexByte(line + 7);

        if (type == 0) { // 数据记录
            uint32_t fullAddr = baseAddr + addr;
            if (fullAddr < flashBase) {
                f_close(&hexFile);
                f_close(&binFile);
                return false;
            }
            uint32_t writePos = fullAddr - flashBase;

            // 如果写位置超出当前文件位置，需要填充0xFF
            if (writePos > currentPos) {
                if (f_lseek(&binFile, currentPos) != FR_OK) {
                    f_close(&hexFile);
                    f_close(&binFile);
                    return false;
                }
                while (currentPos < writePos) {
                    uint8_t fill = 0xFF;
                    UINT bw;
                    if (f_write(&binFile, &fill, 1, &bw) != FR_OK || bw != 1) {
                        f_close(&hexFile);
                        f_close(&binFile);
                        return false;
                    }
                    currentPos++;
                }
            }

            if (writePos != currentPos) {
                if (f_lseek(&binFile, writePos) != FR_OK) {
                    f_close(&hexFile);
                    f_close(&binFile);
                    return false;
                }
                currentPos = writePos;
            }

            // 数据写入
            for (uint8_t i = 0; i < len; i++) {
                uint8_t byte = ParseHexByte(line + 9 + i * 2);
                UINT bw;
                if (f_write(&binFile, &byte, 1, &bw) != FR_OK || bw != 1) {
                    f_close(&hexFile);
                    f_close(&binFile);
                    return false;
                }
                currentPos++;
            }
        } else if (type == 4) { // 扩展线性地址
            baseAddr = ((uint32_t)ParseHexByte(line + 9) << 24) | ((uint32_t)ParseHexByte(line + 11) << 16);
        } else if (type == 1) { // 文件结束
            break;
        }
        // 其他类型忽略
    }

    f_close(&hexFile);
    f_close(&binFile);
    return true;
}