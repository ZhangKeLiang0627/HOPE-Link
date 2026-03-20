/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "u8g2_init.h"
#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FILE_NAME "HOPE.txt"         // 测试文件名
#define FILE_CONTENT "Hello, FATFS!" // 测试写入内容
#define WORK_BUFFER_SIZE 4096        // mkfs 工作缓冲区大小
#define READ_BUF_SIZE 16            // 读文件缓冲区大小
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
u8g2_t disp;
FIL file;        
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static FRESULT fatfs_init(void)
{
  FRESULT res;
  BYTE work_buf[WORK_BUFFER_SIZE]; // mkfs 工作缓冲区

  // 尝试挂载文件系统
  res = f_mount(&USERFatFS, "0:", 1);
  if (res != FR_OK)
  {
    // 挂载失败，执行格式化（FAT格式，簇大小自动适配）
    res = f_mkfs("0:", FM_FAT, 0, work_buf, sizeof(work_buf));
    if (res == FR_OK)
    {
      // 格式化成功后重新挂载
      res = f_mount(&USERFatFS, "0:", 1);
    }
  }

  return res;
}

char read_buf[READ_BUF_SIZE] = {0};

static FRESULT fatfs_rw_test(void)
{
  FRESULT res;
  UINT bytes_written = 0;
  UINT bytes_read = 0;
  size_t content_len = strlen(FILE_CONTENT);

  // 1. 打开/创建文件并写入数据
  res = f_open(&file, FILE_NAME, FA_CREATE_ALWAYS | FA_WRITE);
  if (res == FR_OK)
  {
    // 写入数据（仅写入有效长度，避免多余操作）
    res = f_write(&file, FILE_CONTENT, content_len, &bytes_written);

    // 无论写入成功与否，都要关闭文件（避免资源泄漏）
    f_close(&file);

    // 写入失败直接返回
    if (res != FR_OK)
    {
      return res;
    }
  }
  else
  {
    return res;
  }

  // 2. 打开文件并读取数据
  res = f_open(&file, FILE_NAME, FA_READ);
  if (res == FR_OK)
  {
    // 读取文件内容（预留1字节给结束符）
    res = f_read(&file, read_buf, sizeof(read_buf) - 1, &bytes_read);

    // 关闭文件
    f_close(&file);
  }

  return res;
}
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
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(100);

  FRESULT res;
  res = fatfs_init();
  res = fatfs_rw_test();

  u8g2_init(&disp);
	u8g2_SetFont(&disp, u8g2_font_wqy13_t_gb2312a); 
	u8g2_ClearBuffer(&disp);
	u8g2_DrawUTF8(&disp, 30, 15, "HelloHOPE");
	
  if(res == FR_OK)
  {
    u8g2_DrawUTF8(&disp, 30, 30, "FatFs OK!");
    u8g2_DrawUTF8(&disp, 30, 45, read_buf);
  }
  
  u8g2_SendBuffer(&disp);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
