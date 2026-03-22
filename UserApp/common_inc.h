#ifndef COMMON_INC_H
#define COMMON_INC_H

#define CONFIG_FW_VERSION 1.0

/*---------------------------- C Scope ---------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdio.h>

#include "main.h"
#include "usb_device.h"
#include "CommonMacro.h"

    void Main(void);

    extern uint64_t serialNumber;
    extern char serialNumberStr[13];


#ifdef __cplusplus
}

/*---------------------------- C++ Scope ---------------------------*/
#include "timer.hpp"


#endif
#endif
