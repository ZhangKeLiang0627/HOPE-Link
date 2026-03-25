#ifndef __DAP_CONFIG_H__
#define __DAP_CONFIG_H__


#define CPU_CLOCK               84000000       ///< Specifies the CPU Clock in Hz


#define IO_PORT_WRITE_CYCLES    2               ///< I/O Cycles: 2=default, 1=Cortex-M0+ fast I/0


#define DAP_SWD                 1               ///< SWD Mode:  1 = available, 0 = not available

#define DAP_JTAG                0               ///< JTAG Mode: 0 = not available

#define DAP_JTAG_DEV_CNT        8               ///< Maximum number of JTAG devices on scan chain

#define DAP_DEFAULT_PORT        1               ///< Default JTAG/SWJ Port Mode: 1 = SWD, 2 = JTAG.

#define DAP_DEFAULT_SWJ_CLOCK   4000000         ///< Default SWD/JTAG clock frequency in Hz.


/// Maximum Package Size for Command and Response data.
#define DAP_PACKET_SIZE         64              ///< USB: 64 = Full-Speed, 1024 = High-Speed.

/// Maximum Package Buffers for Command and Response data.
#define DAP_PACKET_COUNT        1              ///< Buffers: 64 = Full-Speed, 4 = High-Speed.


/// Indicate that UART Serial Wire Output (SWO) trace is available.
#define SWO_UART                0               ///< SWO UART:  1 = available, 0 = not available

#define SWO_UART_MAX_BAUDRATE   10000000U       ///< SWO UART Maximum Baudrate in Hz

/// Indicate that Manchester Serial Wire Output (SWO) trace is available.
#define SWO_MANCHESTER          0               ///< SWO Manchester:  1 = available, 0 = not available

#define SWO_BUFFER_SIZE         4096U           ///< SWO Trace Buffer Size in bytes (must be 2^n)
// #define SWO_BUFFER_SIZE         8192U           ///< SWO Trace Buffer Size in bytes (must be 2^n)


/// Debug Unit is connected to fixed Target Device.
#define TARGET_DEVICE_FIXED     0               ///< Target Device: 1 = known, 0 = unknown;



//**************************************************************************************************
/**
JTAG I/O Pin                 | SWD I/O Pin          | CMSIS-DAP Hardware pin mode
---------------------------- | -------------------- | ---------------------------------------------
TCK: Test Clock              | SWCLK: Clock         | Output Push/Pull
TMS: Test Mode Select        | SWDIO: Data I/O      | Output Push/Pull; Input (for receiving data)
TDI: Test Data Input         |                      | Output Push/Pull
TDO: Test Data Output        |                      | Input
nTRST: Test Reset (optional) |                      | Output Open Drain with pull-up resistor
nRESET: Device Reset         | nRESET: Device Reset | Output Open Drain with pull-up resistor

DAP Hardware I/O Pin Access Functions
*/
#include "stm32f4xx_hal.h"
#include "cmsis_compiler.h"


// Configure DAP I/O pins ------------------------------

#define SWCLK_PORT			GPIOA
#define SWCLK_PIN  			GPIO_Pin_5
#define SWDIO_PORT			GPIOA
#define SWDIO_PIN  			GPIO_Pin_6
#define SWDIO_PIN_INDEX		1

#define nRST_PORT			GPIOA
#define nRST_PIN			GPIO_Pin_7

#define LED_CONNECTED_PORT  GPIOC
#define LED_CONNECTED_PIN   GPIO_Pin_13
#define LED_RUNNING_PORT    GPIOC
#define LED_RUNNING_PIN     GPIO_Pin_5


/** Setup JTAG I/O pins: TCK, TMS, TDI, TDO, nTRST, and nRESET.
 - TCK, TMS, TDI, nTRST, nRESET to output mode and set to high level.
 - TDO to input mode.
*/
static void PORT_JTAG_SETUP(void)
{
#if (DAP_JTAG != 0)
#endif
}

/** Setup SWD I/O pins: SWCLK, SWDIO, and nRESET.
 - SWCLK, SWDIO, nRESET to output mode and set to default high level.
*/
static void PORT_SWD_SETUP(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIOA->BSRR = SWCLK_PIN | SWDIO_PIN;

	GPIO_InitStruct.Pin = SWCLK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SWCLK_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SWDIO_PIN;
	HAL_GPIO_Init(SWDIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LED_CONNECTED_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LED_CONNECTED_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = nRST_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(nRST_PORT, &GPIO_InitStruct);
}

/** Disable JTAG/SWD I/O Pins.
 - TCK/SWCLK, TMS/SWDIO, TDI, TDO, nTRST, nRESET to High-Z mode.
*/
static void PORT_OFF(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = SWCLK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SWCLK_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SWDIO_PIN;
	HAL_GPIO_Init(SWDIO_PORT, &GPIO_InitStruct);
}


// SWCLK/TCK I/O pin -------------------------------------

// Current status of the SWCLK/TCK DAP hardware I/O pin
__STATIC_INLINE uint32_t PIN_SWCLK_TCK_IN(void)
{
    return (uint32_t)(SWCLK_PORT->ODR & SWCLK_PIN ? 1 : 0);
}

__STATIC_INLINE void PIN_SWCLK_TCK_SET(void)
{
	SWCLK_PORT->BSRR = SWCLK_PIN;
}

__STATIC_INLINE void PIN_SWCLK_TCK_CLR(void)
{
	SWCLK_PORT->BRR = SWCLK_PIN;
}


// SWDIO/TMS Pin I/O --------------------------------------

// Current status of the SWDIO/TMS DAP hardware I/O pin
__STATIC_INLINE uint32_t PIN_SWDIO_TMS_IN(void)
{
	return (uint32_t)(SWDIO_PORT->ODR & SWDIO_PIN ? 1 : 0);
}

__STATIC_INLINE void PIN_SWDIO_TMS_SET(void)
{
	SWDIO_PORT->BSRR = SWDIO_PIN;
}

__STATIC_INLINE void PIN_SWDIO_TMS_CLR(void)
{
	SWDIO_PORT->BRR = SWDIO_PIN;
}


__STATIC_INLINE uint32_t PIN_SWDIO_IN(void)
{
	return (uint32_t)(SWDIO_PORT->IDR & SWDIO_PIN ? 1 : 0);
}

__STATIC_INLINE void PIN_SWDIO_OUT(uint32_t bit)
{
	if ((bit & 1U) == 1)
		SWDIO_PORT->BSRR = SWDIO_PIN;
	else
		SWDIO_PORT->BRR = SWDIO_PIN;
}

__STATIC_INLINE void PIN_SWDIO_OUT_ENABLE(void)
{
	SWDIO_PORT->CRH &= 0XFFFFFF0F;
	SWDIO_PORT->CRH |= 0X00000030; //设置为输出
}

__STATIC_INLINE void PIN_SWDIO_OUT_DISABLE(void)
{
	SWDIO_PORT->CRH &= 0XFFFFFF0F;
	SWDIO_PORT->CRH |= 0X00000080; //设置成输入
}


// TDI Pin I/O ---------------------------------------------

__STATIC_INLINE uint32_t PIN_TDI_IN(void)
{
#if (DAP_JTAG != 0)
#endif
	return 0;
}

__STATIC_INLINE void PIN_TDI_OUT(uint32_t bit)
{
#if (DAP_JTAG != 0)
#endif
}


// TDO Pin I/O ---------------------------------------------

__STATIC_INLINE uint32_t PIN_TDO_IN(void)
{
#if (DAP_JTAG != 0)
#endif
	return 0;
}


// nTRST Pin I/O -------------------------------------------

__STATIC_INLINE uint32_t PIN_nTRST_IN(void)
{
    return 0;
}

__STATIC_INLINE void PIN_nTRST_OUT(uint32_t bit)
{
}

// nRESET Pin I/O------------------------------------------
__STATIC_INLINE uint32_t PIN_nRESET_IN(void)
{
	return (uint32_t)(nRST_PORT->ODR & nRST_PIN ? 1 : 0);
}

// extern uint8_t swd_write_word(uint32_t addr, uint32_t val);
extern uint8_t swd_init_debug(void);
extern uint8_t swd_write_memory(uint32_t address, uint8_t *data, uint32_t size);
__STATIC_INLINE void PIN_nRESET_OUT(uint32_t bit)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if ((bit & 1U) == 1)
	{
		nRST_PORT->BSRR = nRST_PIN;

		GPIO_InitStruct.Pin = nRST_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(nRST_PORT, &GPIO_InitStruct);
	}
	else
	{
		nRST_PORT->BRR = nRST_PIN;

		GPIO_InitStruct.Pin = nRST_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(nRST_PORT, &GPIO_InitStruct);

		swd_init_debug();
		uint32_t swd_mem_write_data = 0x05FA0000 | 0x4;
		swd_write_memory(0xE000ED0C, (uint8_t *)&swd_mem_write_data, 4);
	}
}


//**************************************************************************************************
/** Connect LED: is active when the DAP hardware is connected to a debugger
    Running LED: is active when program execution in target started
*/

__STATIC_INLINE void LED_CONNECTED_OUT(uint32_t bit)
{
	if ((bit & 1U) == 1)
		LED_CONNECTED_PORT->BRR = LED_CONNECTED_PIN;
	else
		LED_CONNECTED_PORT->BSRR = LED_CONNECTED_PIN;
}

__STATIC_INLINE void LED_RUNNING_OUT(uint32_t bit)
{
	if ((bit & 1U) == 1)
		LED_CONNECTED_PORT->BRR = LED_CONNECTED_PIN;
	else
		LED_CONNECTED_PORT->BSRR = LED_CONNECTED_PIN;
}

__STATIC_INLINE uint32_t TIMESTAMP_GET(void)
{
	return (DWT->CYCCNT);
}

static void DAP_SETUP(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	PORT_SWD_SETUP();
}


static uint32_t RESET_TARGET(void)
{
    return (0);              // change to '1' when a device reset sequence is implemented
}


#endif // __DAP_CONFIG_H__
