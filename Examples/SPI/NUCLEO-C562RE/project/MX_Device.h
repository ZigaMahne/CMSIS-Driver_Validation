/******************************************************************************
 * File Name   : MX_Device.h
 * Date        : 26/08/2025 10:21:14
 * Description : STM32Cube MX parameter definitions
 * Note        : This file is generated with a generator out of the
 *               STM32CubeMX project and its generated files (DO NOT EDIT!)
 ******************************************************************************/

#ifndef MX_DEVICE_H__
#define MX_DEVICE_H__

/* MX_Device.h version */
#define MX_DEVICE_VERSION                       0x01000000

#define STM32_HAL_VERSION_2 

/*------------------------------ SPI1           -----------------------------*/
#define MX_SPI1                                 1

/* Peripheral Clock Frequency */
#define MX_SPI1_PERIPH_CLOCK_FREQ               144000000

/* SPI1_NSS */
#define MX_SPI1_NSS_Pin                         PA4
#define MX_SPI1_NSS_GPIO_Pin                    HAL_GPIO_PIN_4          //GPIO_PIN_4
#define MX_SPI1_NSS_GPIOx                       HAL_GPIOA               //GPIOA
#define MX_SPI1_NSS_GPIO_Mode                   HAL_GPIO_MODE_ALTERNATE //GPIO_MODE_AF_PP
#define MX_SPI1_NSS_GPIO_PuPd                   HAL_GPIO_PULL_NO        //GPIO_NOPULL
#define MX_SPI1_NSS_GPIO_Speed                  HAL_GPIO_SPEED_FREQ_LOW //GPIO_SPEED_FREQ_LOW
#define MX_SPI1_NSS_GPIO_AF                     HAL_GPIO_AF_5           //GPIO_AF5_SPI1

/*------------------------------ USART2          -----------------------------*/
#define MX_USART2                               2

/* Virtual mode */
#define MX_USART2_VM                            VM_ASYNC
#define MX_USART2_VM_ASYNC                      1


#endif  /* MX_DEVICE_H__ */
