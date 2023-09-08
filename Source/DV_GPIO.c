/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -----------------------------------------------------------------------------
 *
 * Project:     CMSIS-Driver Validation
 * Title:       General Input Output (GPIO) Driver Validation tests
 *
 * -----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_dv.h"
#include "DV_GPIO_Config.h"
#include "DV_Framework.h"
#include "Driver_GPIO.h"
#include "cmsis_os2.h"


// Register Driver_GPIO#
extern ARM_DRIVER_GPIO CREATE_SYMBOL(Driver_GPIO, DRV_GPIO);
static ARM_DRIVER_GPIO *drv = &CREATE_SYMBOL(Driver_GPIO, DRV_GPIO);

static uint8_t volatile GPIO_Event = 0U;
static ARM_GPIO_Pin_t volatile  GPIO_Pin = 0U;
static uint8_t volatile IRQ_cnt = 0U;

// Local functions
static void GPIO_DrvEvent (ARM_GPIO_Pin_t pin, uint32_t event);
static void PinPull (ARM_GPIO_Pin_t pin, uint32_t out);
static void PinDisable (ARM_GPIO_Pin_t pin);
static void DriverInit (void);

// GPIO event
static void GPIO_DrvEvent (ARM_GPIO_Pin_t pin, uint32_t event) {
  GPIO_Event |= event;
  GPIO_Pin    = pin;
  IRQ_cnt    += 1U;
}

static void PinPull (ARM_GPIO_Pin_t pin, uint32_t out) {

  drv->SetDirection(pin, ARM_GPIO_OUTPUT);
  drv->SetOutputMode(pin, ARM_GPIO_PUSH_PULL);
  drv->SetPullResistor(pin, ARM_GPIO_PULL_NONE);
  drv->SetOutput(pin, out);
}

static void PinDisable (ARM_GPIO_Pin_t pin) {

  drv->SetOutput(pin, 0U);
  drv->SetDirection(pin, ARM_GPIO_INPUT);
  drv->SetOutputMode(pin, ARM_GPIO_OPEN_DRAIN);
}

static void DriverInit (void) {

  drv->Setup(GPIO_PIN_UNDER_TEST, NULL);
  drv->Setup(GPIO_PIN_AUX, NULL);
}

/*-----------------------------------------------------------------------------
 *      Tests
 *----------------------------------------------------------------------------*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup dv_gpio GPIO Validation
\brief GPIO driver validation
\details
The GPIO validation test performs the following checks:
- API interface compliance.
- Functional settings.
- Event signaling.

To perform GPIO validation tests, it is required to select two pins:
- Pin under test: pin to be tested
- Auxiliary Pin: pin with serial low resistance resistor connected to Pin under test (Suggested resistance of this resistor is around 1 kOhm)

\image html gpio_loopback.png

\note
 - Pins (Pin under test and Auxiliary Pin) should not have any external resistors or any external devices connected to it.

\defgroup gpio_tests Tests
\ingroup dv_gpio

@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_Setup
\details
The function \b GPIO_Setup verifies the GPIO functions in the following order:
  - Setup without callback
  - Setup with callback
*/
void GPIO_Setup (void) {
  int32_t ret;

  /* Setup without callback */
  ret = drv->Setup(GPIO_PIN_UNDER_TEST, NULL);

  if (ret == ARM_GPIO_ERROR_PIN) {
    TEST_FAIL_MESSAGE("[FAILED] Selected pin number is not available!");
  }

  /* Assert Setup function returned status */
  TEST_ASSERT(ret == ARM_DRIVER_OK);

  /* Setup with callback */
  TEST_ASSERT(drv->Setup(GPIO_PIN_UNDER_TEST, GPIO_DrvEvent) == ARM_DRIVER_OK);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetDirection
\details
The function \b GPIO_SetDirection verifies the GPIO direction in the following order:
  - Setup
  - Set Direction parameter error
  - Set pin as Output
  - Set pin as Input
*/
void GPIO_SetDirection (void) {

  /* Setup without callback */
  TEST_ASSERT(drv->Setup(GPIO_PIN_UNDER_TEST, NULL) == ARM_DRIVER_OK);

  /* Set Direction parameter error */
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_OUTPUT + 1) == ARM_DRIVER_ERROR_PARAMETER);

  /* Set Direction as Output */
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_OUTPUT) == ARM_DRIVER_OK);

  /* Set Direction as Input */
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_INPUT) == ARM_DRIVER_OK);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetOutputMode
\details
The function \b GPIO_SetOutputMode verifies the GPIO OutputMode in the following order:
  - Setup
  - Set OutputMode parameter error
  - Set pin as Push-Pull
  - Set pin as Open-Drain
*/
void GPIO_SetOutputMode (void) {

  /* Setup without callback */
  TEST_ASSERT(drv->Setup(GPIO_PIN_UNDER_TEST, NULL) == ARM_DRIVER_OK);

  /* Set OutputMode parameter error */
  TEST_ASSERT(drv->SetOutputMode(GPIO_PIN_UNDER_TEST, ARM_GPIO_OPEN_DRAIN + 1) == ARM_DRIVER_ERROR_PARAMETER);

  /* Set SetOutputMode as Push-Pull */
  TEST_ASSERT(drv->SetOutputMode(GPIO_PIN_UNDER_TEST, ARM_GPIO_PUSH_PULL) == ARM_DRIVER_OK);

  /* Set SetOutputMode as Open-Drain */
  TEST_ASSERT(drv->SetOutputMode(GPIO_PIN_UNDER_TEST, ARM_GPIO_OPEN_DRAIN) == ARM_DRIVER_OK);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetPullResistor
\details
The function \b GPIO_SetPullResistor verifies the GPIO Pull Resistor setings in the following order:
  - Setup
  - Set PullResistor parameter error
  - Set no Pull-Down/Up resistor (Disabled)
  - Set Pull-Down resistor
  - Set Pull-Up resistor
*/
void GPIO_SetPullResistor (void) {

  /* Setup without callback */
  TEST_ASSERT(drv->Setup(GPIO_PIN_UNDER_TEST, NULL) == ARM_DRIVER_OK);

  /* Set PullResistor parameter error */
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_DOWN + 1) == ARM_DRIVER_ERROR_PARAMETER);

  /* Set no Pull-Up/Down resistor (Disabled)*/
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_NONE) == ARM_DRIVER_OK);

  /* Set Pull-Down resistor */
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_DOWN) == ARM_DRIVER_OK);

  /* Set Pull-Up resistor */
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_UP) == ARM_DRIVER_OK);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetEventTrigger
\details
The function \b GPIO_SetEventTrigger verifies the GPIO Trigger setings in the following order:
  - Setup
  - Set Trigger parameter error
  - Set Rising Edge trigger
  - Set Falling Edge trigger
  - Set Either Edge trigger
  - Setup pins
  - Set Rising Edge trigger
  - Trigger Rising Edge with external low resistor
  - Set Falling Edge trigger
  - Trigger Falling Edge with external low resistor
  - Set Either Edge trigger
  - Trigger Either Edge with external low resistor
  - Set Trigger None
  - Trigger Rising/Falling Edge with external low resistor
  - Set Either Edge trigger
  - Disable IRQ
  - Trigger Either Edge with external low resistor
  - Enable IRQ
*/
void GPIO_SetEventTrigger (void) {
  int32_t ret;

  /* Setup without callback */
  TEST_ASSERT(drv->Setup(GPIO_PIN_UNDER_TEST, NULL) == ARM_DRIVER_OK);

  /* Set Trigger parameter error */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_EITHER_EDGE + 1) == ARM_DRIVER_ERROR_PARAMETER);

  /* Set Rising Edge trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_RISING_EDGE) == ARM_DRIVER_OK);

  /* Set Falling Edge trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_FALLING_EDGE) == ARM_DRIVER_OK);

  /* Set Either Edge trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_EITHER_EDGE) == ARM_DRIVER_OK);

  /* Setup pins */
  DriverInit();

  PinPull(GPIO_PIN_AUX, 0U);

  GPIO_Event = 0U;
  GPIO_Pin   = 0U;
  IRQ_cnt    = 0U;

  /* Set Rising Edge trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_RISING_EDGE) == ARM_DRIVER_OK);

  /* Trigger Rising Edge with external low resistor */
  PinPull(GPIO_PIN_AUX, 1U);

  osDelay(100);

  /* Assert that event ARM_GPIO_EVENT_RISING_EDGE was signaled */
  TEST_ASSERT_MESSAGE(GPIO_Event == ARM_GPIO_EVENT_RISING_EDGE, "[FAILED] Event ARM_GPIO_EVENT_RISING_EDGE was not signaled!");

  /* Assert that pin GPIO_PIN_UNDER_TEST was triggered */
  TEST_ASSERT_MESSAGE(GPIO_Pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  /* Assert that number of interrupts was correct */
  TEST_ASSERT_MESSAGE(IRQ_cnt == 1U, "[FAILED] Number of triggered interrupts is incorrect!");

  GPIO_Event = 0U;
  GPIO_Pin   = 0U;
  IRQ_cnt    = 0U;

  /* Set Falling Edge trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_FALLING_EDGE) == ARM_DRIVER_OK);

  /* Trigger Falling Edge with external low resistor */
  PinPull(GPIO_PIN_AUX, 0U);

  osDelay(100);

  /* Assert that event ARM_GPIO_EVENT_FALLING_EDGE was signaled */
  TEST_ASSERT_MESSAGE(GPIO_Event == ARM_GPIO_EVENT_FALLING_EDGE, "[FAILED] Event ARM_GPIO_EVENT_FALLING_EDGE was not signaled!");

  /* Assert that pin GPIO_PIN_UNDER_TEST was triggered */
  TEST_ASSERT_MESSAGE(GPIO_Pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  /* Assert that number of interrupts was correct */
  TEST_ASSERT_MESSAGE(IRQ_cnt == 1U, "[FAILED] Number of triggered interrupts is incorrect!");

  GPIO_Event = 0U;
  GPIO_Pin   = 0U;
  IRQ_cnt    = 0U;

  /* Set Either Edge trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_EITHER_EDGE) == ARM_DRIVER_OK);

  /* Trigger Rising Edge with external low resistor */
  PinPull(GPIO_PIN_AUX, 1U);

  osDelay(100);

  /* Assert that event ARM_GPIO_EVENT_RISING_EDGE or ARM_GPIO_EVENT_EITHER_EDGE was signaled */
  TEST_ASSERT_MESSAGE((GPIO_Event == ARM_GPIO_EVENT_RISING_EDGE) ||
                      (GPIO_Event == ARM_GPIO_EVENT_EITHER_EDGE), "[FAILED] Event ARM_GPIO_EVENT_RISING_EDGE was not signaled!");

  /* Assert that pin GPIO_PIN_UNDER_TEST was triggered */
  TEST_ASSERT_MESSAGE(GPIO_Pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  /* Assert that number of interrupts was correct */
  TEST_ASSERT_MESSAGE(IRQ_cnt == 1U, "[FAILED] Number of triggered interrupts is incorrect!");

  GPIO_Event = 0U;
  GPIO_Pin   = 0U;
  IRQ_cnt    = 0U;

  /* Trigger Falling Edge with external low resistor */
  PinPull(GPIO_PIN_AUX, 0U);

  osDelay(100);

  /* Assert that event ARM_GPIO_EVENT_FALLING_EDGE or ARM_GPIO_EVENT_EITHER_EDGE was signaled */
  TEST_ASSERT_MESSAGE((GPIO_Event == ARM_GPIO_EVENT_FALLING_EDGE) ||
                      (GPIO_Event == ARM_GPIO_EVENT_EITHER_EDGE ), "[FAILED] Event ARM_GPIO_EVENT_FALLING_EDGE was not signaled!");

  /* Assert that pin GPIO_PIN_UNDER_TEST was triggered */
  TEST_ASSERT_MESSAGE(GPIO_Pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  /* Assert that number of interrupts was correct */
  TEST_ASSERT_MESSAGE(IRQ_cnt == 1U, "[FAILED] Number of triggered interrupts is incorrect!");

  GPIO_Event = 0U;
  GPIO_Pin   = 0U;
  IRQ_cnt    = 0U;

  /* Set None trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_NONE) == ARM_DRIVER_OK);

  /* Trigger Rising/Falling Edge with external low resistor */
  PinPull(GPIO_PIN_AUX, 1U);
  PinPull(GPIO_PIN_AUX, 0U);

  osDelay(100);

  /* Assert that no event was triggered */
  TEST_ASSERT_MESSAGE(GPIO_Event== 0U, "[FAILED] Event was triggered!");

  /* Assert that no pin was triggered */
  TEST_ASSERT_MESSAGE( GPIO_Pin == 0U, "[FAILED] Pin GPIO_PIN_UNDER_TEST was triggered!");

  /* Assert that number of interrupts was correct */
  TEST_ASSERT_MESSAGE(IRQ_cnt == 0U, "[FAILED] Interrupt was triggered!");

  /* Set Either Edge trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_EITHER_EDGE) == ARM_DRIVER_OK);

  GPIO_Event = 0U;
  GPIO_Pin   = 0U;
  IRQ_cnt    = 0U;

  __disable_irq();

  /* Trigger Rising Edge with external low resistor */
  PinPull(GPIO_PIN_AUX, 1U);
  PinPull(GPIO_PIN_AUX, 0U);

  __enable_irq();

  osDelay(100);

  /* Assert that event ARM_GPIO_EVENT_RISING_EDGE/ARM_GPIO_EVENT_FALLING_EDGE was signaled */
  TEST_ASSERT_MESSAGE(GPIO_Event & (ARM_GPIO_EVENT_RISING_EDGE | ARM_GPIO_EVENT_FALLING_EDGE) ||
                     (GPIO_Event == ARM_GPIO_EVENT_EITHER_EDGE), "[FAILED] Both Event ARM_GPIO_EVENT_RISING_EDGE and ARM_GPIO_EVENT_FALLING_EDGE was not signaled!");

  /* Assert that pin GPIO_PIN_UNDER_TEST was triggered */
  TEST_ASSERT_MESSAGE(GPIO_Pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  /* Assert that number of interrupts was correct */
  TEST_ASSERT_MESSAGE((IRQ_cnt == 1U) || (IRQ_cnt == 2U), "[FAILED] Number of triggered interrupts is incorrect!");

  /* Disable interrupts */
  drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_NONE);

  /* Disable pin */
  PinDisable(GPIO_PIN_AUX);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetOutput
\details
The function \b GPIO_SetOutput verifies the GPIO output modes in the following order:
  - Setup pins
  - Set pin as Output
  - Set pin as Push-Pull
  - Set Output level LO
  - Get Output level
  - Set Output level HI
  - Get Output level
  - Set pin as Open-Drain
  - Set Output level LO
  - Get Output level
*/
void GPIO_SetOutput (void) {
  uint32_t input;

  /* Setup pins */
  DriverInit();

  /* Set Direction as Output */
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_OUTPUT) == ARM_DRIVER_OK);

   /* Set SetOutputMode as Push-Pull */
  TEST_ASSERT(drv->SetOutputMode(GPIO_PIN_UNDER_TEST, ARM_GPIO_PUSH_PULL) == ARM_DRIVER_OK);

  /* Set Output Level LO */
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 0U);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* Set Output Level HI */
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 1U);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* Set SetOutputMode as Open-Drain */
  TEST_ASSERT(drv->SetOutputMode(GPIO_PIN_UNDER_TEST, ARM_GPIO_OPEN_DRAIN) == ARM_DRIVER_OK);

  /* Set Output Level LO */
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 0U);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_GetInput
\details
The function \b GPIO_GetInput verifies the GPIO input modes in the following order:
  - Setup Pins
  - Set pin as Input
  - Set no Pull-Up/Down resistor (Disabled)
  - External low resistor as Pull-Down
  - Get Output level
  - External low resistor as Pull-Up
  - Get Output level
  - Set Pull-Down resistor
  - Get Output level
  - External low resistor as Pull-Up
  - Get Output level
  - Set Pull-Up resistor
  - Get Output level
  - External low resistor as Pull-Down
  - Get Output level
*/
void GPIO_GetInput (void) {
  uint32_t input;

  /* Setup pins */
  DriverInit();

  /* Set Direction as Input */
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_INPUT) == ARM_DRIVER_OK);

  /* Set no Pull-Up/Down resistor (Disabled)*/
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_NONE) == ARM_DRIVER_OK);

  /* External low resistor as Pull-Down */
  PinPull(GPIO_PIN_AUX, 0U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* External low resistor as Pull-Up */
  PinPull(GPIO_PIN_AUX, 1U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* Disable pin */
  PinDisable(GPIO_PIN_AUX);

  /* Set Pull-Down resistor */
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_DOWN) == ARM_DRIVER_OK);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* External low resistor as Pull-Up */
  PinPull(GPIO_PIN_AUX, 1U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* Disable pin */
  PinDisable(GPIO_PIN_AUX);

  /* Set Pull-Up resistor */
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_UP) == ARM_DRIVER_OK);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* External low resistor as Pull-Down */
  PinPull(GPIO_PIN_AUX, 0U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* Disable pin */
  PinDisable(GPIO_PIN_AUX);
}

/**
@}
*/ 
// end of group dv_gpio
