/*
 * productDef.h
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#ifndef PRODUCTDEF_H_
#define PRODUCTDEF_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

void Error_Handler(void);

/* Definition for USARTx clock resources */
#define MCO_Pin                     GPIO_PIN_0
#define MCO_GPIO_Port               GPIOH
#define STLK_RX_Pin                 GPIO_PIN_8
#define STLK_RX_GPIO_Port           GPIOD
#define STLK_TX_Pin                 GPIO_PIN_9
#define STLK_TX_GPIO_Port           GPIOD
#define USB_PowerSwitchOn_Pin       GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin         GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port   GPIOG
#define USB_SOF_Pin                 GPIO_PIN_8
#define USB_SOF_GPIO_Port           GPIOA
#define USB_VBUS_Pin                GPIO_PIN_9
#define USB_VBUS_GPIO_Port          GPIOA
#define USB_ID_Pin                  GPIO_PIN_10
#define USB_ID_GPIO_Port            GPIOA
#define USB_DM_Pin                  GPIO_PIN_11
#define USB_DM_GPIO_Port            GPIOA
#define USB_DP_Pin                  GPIO_PIN_12
#define USB_DP_GPIO_Port            GPIOA
#define TMS_Pin                     GPIO_PIN_13
#define TMS_GPIO_Port               GPIOA
#define TCK_Pin                     GPIO_PIN_14
#define TCK_GPIO_Port               GPIOA

#define E_NO_EVENT                  0x00000000
#define E_HEARTBEAT                 0x00000001
#define E_REACTION                  0x00000002

#define E_VALID_MASK                (E_HEARTBEAT | E_REACTION)

extern volatile uint32_t gEvents;

#endif /* PRODUCTDEF_H_ */
