#include "hugo_ui_config.h"

// Fatfs
#include "ff.h"

// Json
#include "ArduinoJson.h"

// test
#include "interface_uart.h"

bool HugoUI::LoadConfig(const std::string &file_path)
{
    FIL fil;
    FRESULT fr;
    UINT bytesRead;
    // 使用固定大小的缓冲区，避免动态内存分配
    char buffer[1024];
    long fileSize = 0;
    
    // 1. 使用fatfs打开目标文件
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
    
    // 2. ArduinoJson，创建doc，从目标文件读取完整内容，解析json
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, buffer);
    
    if (error) {
        return false; // JSON解析失败
    }
    
    Usart_debugMsg("LoadConfig: %s", buffer);
    // 这里可以添加从JSON中提取数据的代码
    // 例如：toneVolume = doc["system"]["tone_volume"];
    //      encoderDir = doc["system"]["encoder_dir"];
    //      dapAddress = doc["dap"]["address"];
    
    return true; // 成功加载配置
}

bool HugoUI::SaveConfig(const std::string &file_path)
{
    FIL fil;
    FRESULT fr;
    UINT bytesWritten;
    
    // 1. 使用fatfs打开目标文件
    fr = f_open(&fil, (const TCHAR*)file_path.c_str(), FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        return false; // 无法打开或创建文件
    }
    
    // 2. ArduinoJson，创建doc，按照指定结构创建
    StaticJsonDocument<256> doc;
    
    // 设置系统配置
    doc["system"] = JsonObject();
    doc["system"]["tone_volume"] = 80;      // 默认音量
    doc["system"]["encoder_dir"] = true;    // 默认编码器方向
    
    // 设置DAP配置
    doc["dap"] = JsonObject();
    doc["dap"]["address"] = 0x08000000;     // 默认DAP地址
    
    // 序列化JSON
    std::string jsonString;
    serializeJson(doc, jsonString);
    
    // 3. 将doc内容写入目标file_path
    fr = f_write(&fil, jsonString.c_str(), jsonString.length(), &bytesWritten);
    f_close(&fil);
    Usart_debugMsg("SaveConfig: %s", jsonString.c_str());
    
    if (fr != FR_OK || bytesWritten != jsonString.length()) {
        return false; // 写入失败
    }
    
    return true; // 成功保存配置
}