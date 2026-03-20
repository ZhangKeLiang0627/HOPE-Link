#include "main.h"
#include "w25qxx.h"
#include <string.h>

#ifdef DEBUG
#include <stdlib.h>
#include <stdio.h>
#endif

#define W25QXX_SECTOR_BUF_SIZE 4096 // 扇区大小4096字节
static uint8_t W25QXX_SECTOR_BUFFER[W25QXX_SECTOR_BUF_SIZE];

/*
 * Internal functions
 */

/**
 * @brief  Enables CS (driving it low) of the W25Qxx
 *
 * @param  W25Qxx handle
 * @retval None
 */
static inline void cs_on(W25QXX_HandleTypeDef *w25qxx) {
#ifndef W25QXX_QSPI
    HAL_GPIO_WritePin(w25qxx->cs_port, w25qxx->cs_pin, GPIO_PIN_RESET);
#endif
}

/**
 * @brief  Disables CS (driving it high) of the W25Qxx
 *
 * @param  W25Qxx handle
 * @retval None
 */
static inline void cs_off(W25QXX_HandleTypeDef *w25qxx) {
    HAL_GPIO_WritePin(w25qxx->cs_port, w25qxx->cs_pin, GPIO_PIN_SET);
}

/**
 * @brief  Transmit data to w25qxx - ignore returned data
 *
 * @param  W25Qxx handle
 * @param  Pointer to buffer with data to transmit
 * @param  Length (in bytes) of data to be transmitted
 * @retval None
 */
W25QXX_result_t w25qxx_transmit(W25QXX_HandleTypeDef *w25qxx, uint8_t *buf, uint32_t len) {
    W25QXX_result_t ret = W25QXX_Err;
    if (HAL_SPI_Transmit(w25qxx->spiHandle, buf, len, HAL_MAX_DELAY) == HAL_OK) {
        ret = W25QXX_Ok;
    }
    return ret;
}

/*
 * Receive data from w25qxx
 */
W25QXX_result_t w25qxx_receive(W25QXX_HandleTypeDef *w25qxx, uint8_t *buf, uint32_t len) {
    W25QXX_result_t ret = W25QXX_Err;
    if (HAL_SPI_Receive(w25qxx->spiHandle, buf, len, HAL_MAX_DELAY) == HAL_OK) {
        ret = W25QXX_Ok;
    }
    return ret;
}

uint32_t w25qxx_read_id(W25QXX_HandleTypeDef *w25qxx) {
    uint32_t ret = 0;
    uint8_t buf[3];
    cs_on(w25qxx);
    buf[0] = W25QXX_GET_ID;
    if (w25qxx_transmit(w25qxx, buf, 1) == W25QXX_Ok) {
        if (w25qxx_receive(w25qxx, buf, 3) == W25QXX_Ok) {
            ret = (uint32_t) ((buf[0] << 16) | (buf[1] << 8) | (buf[2]));
        }
    }
    cs_off(w25qxx);
    return ret;
}

uint8_t w25qxx_get_status(W25QXX_HandleTypeDef *w25qxx) {
    uint8_t ret = 0;
    uint8_t buf = W25QXX_READ_REGISTER_1;
    cs_on(w25qxx);
    if (w25qxx_transmit(w25qxx, &buf, 1) == W25QXX_Ok) {
        if (w25qxx_receive(w25qxx, &buf, 1) == W25QXX_Ok) {
            ret = buf;
        }
    }
    cs_off(w25qxx);
    return ret;
}

W25QXX_result_t w25qxx_write_enable(W25QXX_HandleTypeDef *w25qxx) {
    W25_DBG("w25qxx_write_enable");
    W25QXX_result_t ret = W25QXX_Err;
    uint8_t buf[1];
    cs_on(w25qxx);
    buf[0] = W25QXX_WRITE_ENABLE;
    if (w25qxx_transmit(w25qxx, buf, 1) == W25QXX_Ok) {
        ret = W25QXX_Ok;
    }
    cs_off(w25qxx);
    return ret;
}

W25QXX_result_t w25qxx_wait_for_ready(W25QXX_HandleTypeDef *w25qxx, uint32_t timeout) {
    W25QXX_result_t ret = W25QXX_Ok;
    uint32_t begin = HAL_GetTick();
    uint32_t now = HAL_GetTick();
    // Wait until the busy flags disappear.
    while ((now - begin <= timeout) && (w25qxx_get_status(w25qxx) & 0x01)) {
        now = HAL_GetTick();
    }
    if (now - begin == timeout)
        ret = W25QXX_Timeout;
    return ret;
}

#ifdef W25QXX_QSPI
W25QXX_result_t w25qxx_init(W25QXX_HandleTypeDef *w25qxx, QSPI_HandleTypeDef *qhspi) {
#else
W25QXX_result_t w25qxx_init(W25QXX_HandleTypeDef *w25qxx, SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin) {
#endif

    W25QXX_result_t result = W25QXX_Ok;

    W25_DBG("w25qxx_init");

    // char *version_buffer = malloc(strlen(W25QXX_VERSION) + 1);
    // if (version_buffer) {
    //     sprintf(version_buffer, "%s", W25QXX_VERSION);
    //     free(version_buffer);
    // }

#ifdef W25QXX_QSPI
    w25qxx->qspiHandle = qhspi;
#else
    w25qxx->spiHandle = hspi;
    w25qxx->cs_port = cs_port;
    w25qxx->cs_pin = cs_pin;

    cs_off(w25qxx);
#endif

    uint32_t id = w25qxx_read_id(w25qxx);
    if (id) {
        w25qxx->manufacturer_id = (uint8_t) (id >> 16);
        w25qxx->device_id = (uint16_t) (id & 0xFFFF);

        switch (w25qxx->manufacturer_id) {
        case W25QXX_MANUFACTURER_GIGADEVICE:

            w25qxx->block_size = 0x10000;
            w25qxx->sector_size = 0x1000;
            w25qxx->sectors_in_block = 0x10;
            w25qxx->page_size = 0x100;
            w25qxx->pages_in_sector = 0x10;

            switch (w25qxx->device_id) {
            case 0x6017:
                w25qxx->block_count = 0x80;
                break;
            default:
                W25_DBG("Unknown Giga Device device");
                result = W25QXX_Err;
            }

            break;
        case W25QXX_MANUFACTURER_WINBOND:

            w25qxx->block_size = 0x10000;
            w25qxx->sector_size = 0x1000;
            w25qxx->sectors_in_block = 0x10;
            w25qxx->page_size = 0x100;
            w25qxx->pages_in_sector = 0x10;

            switch (w25qxx->device_id) {
            case 0x4018:
                w25qxx->block_count = 0x100;
                break;
            case 0x4017:
                w25qxx->block_count = 0x80;
                break;
            case 0x4016:
                w25qxx->block_count = 0x40;
                break;
            default:
                W25_DBG("Unknown Winbond device");
                result = W25QXX_Err;
            }

            break;
        default:
            W25_DBG("Unknown manufacturer");
            result = W25QXX_Err;
        }
    } else {
        result = W25QXX_Err;
    }

    if (result == W25QXX_Err) {
        // Zero the handle so it is clear initialization failed!
        memset(w25qxx, 0, sizeof(W25QXX_HandleTypeDef));
    }

    return result;

}

W25QXX_result_t w25qxx_read(W25QXX_HandleTypeDef *w25qxx, uint32_t address, uint8_t *buf, uint32_t len) {

    W25_DBG("w25qxx_read - address: 0x%08lx, lengh: 0x%04lx", address, len);

    // Transmit buffer holding command and address
    uint8_t tx[4] = {
    W25QXX_READ_DATA,
            (uint8_t) (address >> 16),
            (uint8_t) (address >> 8),
            (uint8_t) (address)
    };

    // First wait for device to get ready
    if (w25qxx_wait_for_ready(w25qxx, HAL_MAX_DELAY) != W25QXX_Ok) {
        return W25QXX_Err;
    }

    cs_on(w25qxx);
    if (w25qxx_transmit(w25qxx, tx, 4) == W25QXX_Ok) { // size will always be fixed
        if (w25qxx_receive(w25qxx, buf, len) != W25QXX_Ok) {
            cs_off(w25qxx);
            return W25QXX_Err;
        }
    }
    cs_off(w25qxx);

    return W25QXX_Ok;
}

W25QXX_result_t w25qxx_write(W25QXX_HandleTypeDef *w25qxx, uint32_t address, uint8_t *buf, uint32_t len) {

    W25_DBG("w25qxx_write - address 0x%08lx len 0x%04lx", address, len);

    // Let's determine the pages
    uint32_t first_page = address / w25qxx->page_size;
    uint32_t last_page = (address + len - 1) / w25qxx->page_size;

    W25_DBG("w25qxx_write %lu pages from %lu to %lu", 1 + last_page - first_page, first_page, last_page);

    uint32_t buffer_offset = 0;
    uint32_t start_address = address;

    for (uint32_t page = first_page; page <= last_page; ++page) {

        uint32_t write_len = w25qxx->page_size - (start_address & (w25qxx->page_size - 1));
        write_len = len > write_len ? write_len : len;

        W25_DBG("w25qxx_write: handling page %lu start_address = 0x%08lx buffer_offset = 0x%08lx len = %04lx", page, start_address, buffer_offset, write_len);

        // First wait for device to get ready
        if (w25qxx_wait_for_ready(w25qxx, HAL_MAX_DELAY) != W25QXX_Ok) {
            return W25QXX_Err;
        }

        if (w25qxx_write_enable(w25qxx) == W25QXX_Ok) {

            uint8_t tx[4] = {
            W25QXX_PAGE_PROGRAM, (uint8_t) (start_address >> 16), (uint8_t) (start_address >> 8), (uint8_t) (start_address), };

            cs_on(w25qxx);
            if (w25qxx_transmit(w25qxx, tx, 4) == W25QXX_Ok) { // size will always be fixed
                // Now write the buffer
                if (w25qxx_transmit(w25qxx, buf + buffer_offset, write_len) != W25QXX_Ok) {
                    cs_off(w25qxx);
                    return W25QXX_Err;
                }
            }
            cs_off(w25qxx);
        }
        start_address += write_len;
        buffer_offset += write_len;
        len -= write_len;
    }

    return W25QXX_Ok;
}

W25QXX_result_t w25qxx_erase(W25QXX_HandleTypeDef *w25qxx, uint32_t address, uint32_t len) {

    W25_DBG("w25qxx_erase, address = 0x%08lx len = 0x%04lx", address, len);

    W25QXX_result_t ret = W25QXX_Ok;

    // Let's determine the sector start
    uint32_t first_sector = address / w25qxx->sector_size;
    uint32_t last_sector = (address + len - 1) / w25qxx->sector_size;

    W25_DBG("w25qxx_erase: first sector: 0x%04lx", first_sector);W25_DBG("w25qxx_erase: last sector : 0x%04lx", last_sector);

    for (uint32_t sector = first_sector; sector <= last_sector; ++sector) {

        W25_DBG("Erasing sector %lu, starting at: 0x%08lx", sector, sector * w25qxx->sector_size);

        // First we have to ensure the device is not busy
        if (w25qxx_wait_for_ready(w25qxx, HAL_MAX_DELAY) == W25QXX_Ok) {
            if (w25qxx_write_enable(w25qxx) == W25QXX_Ok) {

                uint32_t sector_start_address = sector * w25qxx->sector_size;

                uint8_t tx[4] = {
                W25QXX_SECTOR_ERASE, (uint8_t) (sector_start_address >> 16), (uint8_t) (sector_start_address >> 8), (uint8_t) (sector_start_address), };

                cs_on(w25qxx);
                if (w25qxx_transmit(w25qxx, tx, 4) != W25QXX_Ok) {
                    ret = W25QXX_Err;
                }
                cs_off(w25qxx);
            }
        } else {
            ret = W25QXX_Timeout;
        }

    }

    return ret;
}

/**
 * @brief  w25qxx_write_with_erase / 智能擦写
 * 1. 读取扇区原有数据 
 * 2. 校验待写入区域是否全为0XFF
 * 3. 仅擦除需要改写的扇区 
 * 4. 合并新旧数据写入
 *
 * @param  w25qxx: W25QXX句柄
 * @param  address: 起始写入地址（24bit）
 * @param  buf: 待写入数据缓冲区
 * @param  len: 要写入的字节数
 * @retval W25QXX_result_t: 操作结果（W25QXX_Ok/W25QXX_Err/W25QXX_Timeout）
 */
W25QXX_result_t w25qxx_write_with_erase(W25QXX_HandleTypeDef *w25qxx, uint32_t address, uint8_t *buf, uint32_t len)
{
    // 1. 入参合法性检查
    if (w25qxx == NULL || buf == NULL || len == 0 || w25qxx->sector_size != W25QXX_SECTOR_BUF_SIZE)
    {
        W25_DBG("w25qxx_write_with_erase: invalid parameters");
        return W25QXX_Err;
    }

    uint32_t secpos;               // 写入地址所在的扇区编号
    uint32_t secoff;               // 写入地址在扇区内的偏移
    uint32_t secremain;            // 扇区内剩余可写入的字节数
    uint32_t write_len;            // 当前次要写入的字节数
    uint8_t *pbuf = buf;           // 数据缓冲区指针
    uint32_t write_addr = address; // 当前写入地址
    uint32_t remain_len = len;     // 剩余待写入字节数

    // 2. 按扇区循环处理
    while (remain_len > 0)
    {
        // 计算当前写入地址对应的扇区信息
        secpos = write_addr / w25qxx->sector_size;                     // 扇区编号 = 地址 / 扇区大小
        secoff = write_addr % w25qxx->sector_size;                     // 扇区内偏移 = 地址 % 扇区大小
        secremain = w25qxx->sector_size - secoff;                      // 扇区剩余空间 = 扇区大小 - 偏移
        write_len = (remain_len < secremain) ? remain_len : secremain; // 本次写入长度（不超过剩余空间）

        // 3. 读取当前扇区的原有数据
        if (w25qxx_read(w25qxx, secpos * w25qxx->sector_size, W25QXX_SECTOR_BUFFER, w25qxx->sector_size) != W25QXX_Ok)
        {
            W25_DBG("w25qxx_write_with_erase: read sector failed (sec:%lu)", secpos);
            return W25QXX_Err;
        }

        // 4. 校验扇区内待写入区域是否全为0XFF（判断是否需要擦除）
        uint8_t need_erase = 0;
        for (uint32_t i = 0; i < write_len; i++)
        {
            if (W25QXX_SECTOR_BUFFER[secoff + i] != 0XFF)
            {
                need_erase = 1; // 存在非0XFF数据，需要擦除扇区
                break;
            }
        }

        // 5. 若需要擦除，则先擦除当前扇区
        if (need_erase)
        {
            if (w25qxx_erase(w25qxx, secpos * w25qxx->sector_size, w25qxx->sector_size) != W25QXX_Ok)
            {
                W25_DBG("w25qxx_write_with_erase: erase sector failed (sec:%lu)", secpos);
                return W25QXX_Err;
            }
            // 擦除后扇区全为0XFF，直接覆盖待写入区域的数据
            for (uint32_t i = 0; i < write_len; i++)
            {
                W25QXX_SECTOR_BUFFER[secoff + i] = pbuf[i];
            }
            // 写入整个扇区
            if (w25qxx_write(w25qxx, secpos * w25qxx->sector_size, W25QXX_SECTOR_BUFFER, w25qxx->sector_size) != W25QXX_Ok)
            {
                W25_DBG("w25qxx_write_with_erase: write sector failed (sec:%lu)", secpos);
                return W25QXX_Err;
            }
        }
        else
        {
            // 无需擦除，直接写入当前区域（和第一个库 W25QXX_Write_Page 逻辑一致）
            if (w25qxx_write(w25qxx, write_addr, pbuf, write_len) != W25QXX_Ok)
            {
                W25_DBG("w25qxx_write_with_erase: write data failed (addr:0x%08lx)", write_addr);
                return W25QXX_Err;
            }
        }

        // 6. 更新指针和长度，处理下一段数据
        remain_len -= write_len; // 剩余长度递减
        write_addr += write_len; // 写入地址后移
        pbuf += write_len;       // 数据指针后移
    }

    W25_DBG("w25qxx_write_with_erase: success (addr:0x%08lx, len:0x%04lx)", address, len);
    return W25QXX_Ok;
}

W25QXX_result_t w25qxx_chip_erase(W25QXX_HandleTypeDef *w25qxx) {
    if (w25qxx_write_enable(w25qxx) == W25QXX_Ok) {
        uint8_t tx[1] = {
        W25QXX_CHIP_ERASE };
        cs_on(w25qxx);
        if (w25qxx_transmit(w25qxx, tx, 1) != W25QXX_Ok) {
            return W25QXX_Err;
        }
        cs_off(w25qxx);
        if (w25qxx_wait_for_ready(w25qxx, HAL_MAX_DELAY) != W25QXX_Ok) {
            return W25QXX_Err;
        }
    }
    return W25QXX_Ok;
}

// user's handle
W25QXX_HandleTypeDef w25qxx;
