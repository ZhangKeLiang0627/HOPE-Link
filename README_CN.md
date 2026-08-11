# HOPE-Link

# 项目简介

一款带图形界面的**离线 SWD 下载器**（基于 CMSIS-DAP），可通过板载 OLED + 编码器 + 按键操作，对目标芯片进行离线烧录。

硬件开源地址：https://oshwhub.com/hugego/hope-stm32f401-based-mini-devframework

# 硬件资源

## 主控芯片

| 项目 | 参数 |
| ---- | ---- |
| 主控芯片 | STM32F401RET6 — ARM Cortex-M4F @ 84 MHz，LQFP64，512 KB Flash，96 KB SRAM |
| 时钟 | 外部 25 MHz 晶振，PLL 倍频至 84 MHz |

## 引脚与外设

| 引脚 | 外设 | 功能 |
| ---- | ---- | ---- |
| PA5 | GPIO — DAP_SWCLK | 目标板 SWD 时钟 |
| PA6 | GPIO — DAP_SWDIO | 目标板 SWD 数据收发 |
| PA7 | GPIO — DAP_nRST | 目标板复位（开漏） |
| PA2 | TIM5_CH3 + DMA | WS2812B RGB 指示灯（800 kHz PWM） |
| PA3 | TIM9_CH2 | 蜂鸣器（PWM 发声） |
| PB0 / PB1 | GPIO — 软件 I2C | MPU6050 惯性传感器 |
| PB4 / PB5 | TIM3（编码器模式） | 旋转编码器 |
| PB6 / PB7 | I2C1 | OLED 128×64（SSD1312）显示屏 |
| PB12 | GPIO — SPI2_CS | W25Q128 片选 |
| PB13 – PB15 | SPI2 | W25Q128 SPI NOR Flash（FATFS 文件系统 + USB 磁盘） |
| PA9 / PA10 | USART1 | 调试串口 + 画面投屏 |
| PA11 / PA12 | USB OTG FS | USB 大容量存储设备（MSC） |
| PA13 / PA14 | SWD（SYS） | 板载调试烧录口 |
| PC13 | GPIO — LED | 状态指示灯 |
| PC2 | GPIO — KEY2 | 按键 |
| PH0 / PH1 | RCC HSE | 外部 25 MHz 晶振 |

# 编译

keil535 + CMSIS 5.8.0 + STM32F4 DFP Pack 3.1.1

# 快照

![](4.Pics/0349ade29bd411eb8159044fc184cf3d.jpg)
![](4.Pics/8d7a690032cc7baf06612775c516cc72.jpg)

# 开源协议

MIT.
