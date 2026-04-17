#include "hugo_ui_config.h"
#include "ff.h"             // Fatfs
#include "ArduinoJson.h"    // Json
#include "interface_uart.h" // test

// 创建目录
static bool CreateDirectory(const TCHAR* path)
{
    FILINFO fno;
    FRESULT fr = f_stat(path, &fno);
    
    if (fr != FR_OK || !(fno.fattrib & AM_DIR)) {
        // 目录不存在，尝试创建
        fr = f_mkdir(path);
        if (fr != FR_OK) {
            Usart_debugMsg("[Config] Failed to create directory: %s", path);
            return false;
        }
        Usart_debugMsg("[Config] Directory created: %s", path);
    }
    
    return true;
}

// 确保所有必需的目录存在
static bool EnsureDirectoriesExist(void)
{
    // 创建根目录 0:/
    if (!CreateDirectory(_T("0:/"))) {
        return false;
    }
    
    // 创建子目录
    const TCHAR* dirs[] = {
        _T("0:/Firmware"),
        _T("0:/Config"),
        _T("0:/Resource"),
        _T("0:/Certs")
    };
    
    for (const auto& dir : dirs) {
        if (!CreateDirectory(dir)) {
            return false;
        }
    }
    
    return true;
}

// 解析JSON字符串，提取参数
static bool LoadParams(const char* jsonBuffer)
{
    // 从目标文件读取完整内容，解析json
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonBuffer);
    
    if (error) {
        return false; // JSON解析失败
    }
    
    Usart_debugMsg("LoadParams: %s", jsonBuffer);
    // 这里可以添加从JSON中提取数据的代码
    // 例如：toneVolume = doc["system"]["tone_volume"];
    //      encoderDir = doc["system"]["encoder_dir"];
    //      dapAddress = doc["dap"]["address"];
    
    return true; // 成功加载参数
}

// 生成JSON字符串
static bool SaveParams(std::string& jsonString)
{
    StaticJsonDocument<256> doc;
    // 设置系统配置
    doc["system"] = JsonObject();
    doc["system"]["tone_volume"] = 80;      // 默认音量
    doc["system"]["encoder_dir"] = true;    // 默认编码器方向
    // 设置DAP配置
    doc["dap"] = JsonObject();
    doc["dap"]["address"] = 0x08000000;     // 默认DAP地址
    
    // 序列化JSON
    serializeJson(doc, jsonString);
    
    return true; // 成功生成JSON字符串
}

bool HugoUI::LoadConfig(const std::string &file_path)
{
    FIL fil;
    FRESULT fr;
    UINT bytesRead;
    uint8_t buffer[1024] = {0};
    uint32_t fileSize = 0;
    
    // 确保所有必需的目录存在
    if (!EnsureDirectoriesExist()) {
        Usart_debugMsg("[Config] Failed to create required directories");
        return false;
    }
    
    // 打开目标文件
    fr = f_open(&fil, (const TCHAR*)file_path.c_str(), FA_READ);
    
    if (fr != FR_OK) {
        // 文件不存在或无法打开，直接调用SaveConfig创建默认配置
        if (!SaveConfig(file_path)) {
            return false; // 无法创建默认配置
        }
        
        // 重新打开文件以读取刚写入的默认配置
        fr = f_open(&fil, (const TCHAR*)file_path.c_str(), FA_READ);
        if (fr != FR_OK) {
            return false; // 无法重新打开文件
        }
    }
    
    // 获取文件大小
    fileSize = f_size(&fil);
    // 检查文件大小是否超过缓冲区
    if (fileSize >= sizeof(buffer)) {
        f_close(&fil);
        return false; // 文件过大
    }
    
    // 读取文件内容
    fr = f_read(&fil, buffer, fileSize, &bytesRead);
    f_close(&fil);
    if (fr != FR_OK || bytesRead != fileSize) {
        return false; // 读取失败
    }
    buffer[fileSize] = '\0'; // 确保字符串终止
    
    // log
    Usart_debugMsg("LoadConfig: %s", buffer);

    // LoadParams
    return LoadParams((const char*)buffer);
}

bool HugoUI::SaveConfig(const std::string &file_path)
{
    FIL fil;
    FRESULT fr;
    UINT bytesWritten;
    
    // 确保所有必需的目录存在
    if (!EnsureDirectoriesExist()) {
        Usart_debugMsg("[Config] Failed to create required directories");
        return false;
    }
    
    // SaveParams
    std::string jsonString;
    if (!SaveParams(jsonString)) {
        return false; // 生成JSON失败
    }
    
    // 打开目标文件
    fr = f_open(&fil, (const TCHAR*)file_path.c_str(), FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        return false; // 无法打开或创建文件
    }
    
    // 将json字符串写入目标文件
    fr = f_write(&fil, jsonString.c_str(), jsonString.length(), &bytesWritten);
    f_close(&fil);    
    if (fr != FR_OK || bytesWritten != jsonString.length()) {
        return false; // 写入失败
    }

    // log
    Usart_debugMsg("SaveConfig: %s", jsonString.c_str());
    
    // 成功保存配置
    return true; 
}