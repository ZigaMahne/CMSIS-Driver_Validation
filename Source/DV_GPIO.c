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
  drv->Setup(GPIO_PIN_LOWER, NULL);
  drv->Setup(GPIO_PIN_HIGHER, NULL);
}

/*-----------------------------------------------------------------------------
 *      Tests
 *----------------------------------------------------------------------------*/
/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_Setup
\details
The test Setup \b GPIO_setup verifies the GPIO functions with the sequence:
  - \b Setup without callback
  - \b Setup with callback
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
The test Direction \b GPIO_SetDirection verifies the GPIO direction with the sequence:
  - \b Setup
  - \b Set Direction parameter error
  - \b Set pin as Output
  - \b Set pin as Input
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
The test OutputMode \b GPIO_SetOutputMode verifies the GPIO OutputMode with the sequence:
  - \b Setup
  - \b Set OutputMode parameter error
  - \b Set pin as Push-Pull
  - \b Set pin as Open-Drain
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
The test Pull Resistor \b GPIO_SetPullResistor verifies the GPIO Pull Resistor seting with the sequence:
  - \b Setup
  - \b Set PullResistor parameter error
  - \b Set no Pull-Down/Up resistor (Disabled)
  - \b Set Pull-Down resistor
  - \b Set Pull-Up resistor
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
The test Trigger Rising Edge \b GPIO_SetEventTrigger verifies the GPIO Trigger with the sequence:
  - \b Setup
  - \b Set Trigger parameter error
  - \b Set Rising Edge trigger
  - \b Set Falling Edge trigger
  - \b Set Either Edge trigger

*/
void GPIO_SetEventTrigger (void) {

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
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: SetEventTrigger
\details
The test Trigger Rising Edge \b SetEventTrigger verifies the GPIO Trigger with the sequence:
  - \b Setup pins
  - \b Set Rising Edge trigger
  - \b Trigger Rising Edge with external resistor
  - \b Set Falling Edge trigger
  - \b Trigger Falling Edge with external resistor
  - \b Set Either Edge trigger
  - \b Trigger Either Edge with external resistor
  - \b Set Trigger None
  - \b Trigger Rising/Falling Edge with external resistor
  - \b Set Either Edge trigger
  - \b Disable IRQ
  - \b Trigger Either Edge with external resistor
  - \b Enable IRQ
*/
void SetEventTrigger (void) {
  int32_t ret;

  /* Setup pins */
  DriverInit();

  PinPull(GPIO_PIN_LOWER, 0U);

  GPIO_Event = 0U;
  GPIO_Pin   = 0U;
  IRQ_cnt    = 0U;

  /* Set Rising Edge trigger */
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_RISING_EDGE) == ARM_DRIVER_OK);

  /* Trigger Rising Edge with external resistor */
  PinPull(GPIO_PIN_LOWER, 1U);

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

  /* Trigger Falling Edge with external resistor */
  PinPull(GPIO_PIN_LOWER, 0U);

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

  /* Trigger Rising Edge with external resistor */
  PinPull(GPIO_PIN_LOWER, 1U);

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

  /* Trigger Falling Edge with external resistor */
  PinPull(GPIO_PIN_LOWER, 0U);

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

  /* Trigger Rising/Falling Edge with external resistor */
  PinPull(GPIO_PIN_LOWER, 1U);
  PinPull(GPIO_PIN_LOWER, 0U);

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

  /* Trigger Rising Edge with external resistor */
  PinPull(GPIO_PIN_LOWER, 1U);
  PinPull(GPIO_PIN_LOWER, 0U);

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
  PinDisable(GPIO_PIN_LOWER);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: SetOutput
\details
The test Output Level \b SetOutput verifies the GPIO output with the sequence:
  - \b Setup pins
  - \b Set pin as Output
  - \b Set pin as Push-Pull
  - \b Set Output level LO
  - \b Get Output level
  - \b Set Output level HI
  - \b Get Output level

  - \b Set pin as Open-Drain
  - \b Set Output level LO
  - \b Get Output level
  - \b Set Output level HI
  - \b External high resistor as Pull-Up
  - \b Get Output level
  - \b Set Output level LO
  - \b External low resistor as Pull-Down
  - \b Get Output level
*/
void SetOutput (void) {
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

  /* Set Output Level HI */
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 1U);

  /* External high resistor as Pull-Up */
  PinPull(GPIO_PIN_HIGHER, 1U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* Set Output Level LO */
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 0U);

  /* Disable pin */
  PinDisable(GPIO_PIN_HIGHER);

  /* External low resistor as Pull-Down */
  PinPull(GPIO_PIN_LOWER, 0U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* Disable pin */
  PinDisable(GPIO_PIN_LOWER);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GetInput
\details
The test Input Low level \b GetInput verifies the GPIO input with the sequence:
  - \b Setup Pins
  - \b Set pin as Input
  - \b Set no Pull-Up/Down resistor (Disabled)
  - \b External resistor with high resistor as Pull-Down
  - \b Get Output level
  - \b External high resistor as Pull-Up
  - \b Get Output level

  - \b Set Pull-Down resistor
  - \b Get Output level
  - \b External high resistor as Pull-Up
  - \b Get Output level
  - \b Set Pull-Up resistor
  - \b Get Output level
  - \b External resistor with high resistor as Pull-Down
  - \b Get Output level

  - \b External low resistor as Pull-Down
  - \b Get Output level
  - \b Set Pull-Down resistor
  - \b External resistor with low resistor as Pull-Up
  - \b Get Output level
*/
void GetInput (void) {
  uint32_t input;

  /* Setup pins */
  DriverInit();

  /* Set Direction as Input */
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_INPUT) == ARM_DRIVER_OK);

  /* Set no Pull-Up/Down resistor (Disabled)*/
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_NONE) == ARM_DRIVER_OK);

  /* External resistor with high resistor as Pull-Down */
  PinPull(GPIO_PIN_HIGHER, 0U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* External high resistor as Pull-Up */
  PinPull(GPIO_PIN_HIGHER, 1U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* Disable pin */
  PinDisable(GPIO_PIN_HIGHER);

  /* Set Pull-Down resistor */
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_DOWN) == ARM_DRIVER_OK);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* External high resistor as Pull-Up */
  PinPull(GPIO_PIN_HIGHER, 1U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* Disable pin */
  PinDisable(GPIO_PIN_HIGHER);

  /* Set Pull-Up resistor */
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_UP) == ARM_DRIVER_OK);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* External resistor with high resistor as Pull-Down */
  PinPull(GPIO_PIN_HIGHER, 0U);

  osDelay(100);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* Disable pin */
  PinDisable(GPIO_PIN_HIGHER);

  /* External low resistor as Pull-Down */
  PinPull(GPIO_PIN_LOWER, 0U);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 0U);

  /* Set Pull-Down resistor */
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_DOWN) == ARM_DRIVER_OK);

  /* External resistor with low resistor as Pull-Up */
  PinPull(GPIO_PIN_LOWER, 1U);

  /* Get Output */
  input = drv->GetInput(GPIO_PIN_UNDER_TEST);
  TEST_ASSERT(input == 1U);

  /* Disable pin */
  PinDisable(GPIO_PIN_LOWER);
}

/**
@}
*/ 
// end of group dv_gpio
