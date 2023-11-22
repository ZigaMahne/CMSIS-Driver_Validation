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
 * Title:       General-purpose Input Output (GPIO) Driver Validation tests
 *
 * -----------------------------------------------------------------------------
 */

#ifndef __DOXYGEN__                     // Exclude form the documentation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_dv.h"
#include "DV_GPIO_Config.h"
#include "DV_Framework.h"

#include "Driver_GPIO.h"

// Register Driver_GPIO#
extern ARM_DRIVER_GPIO          CREATE_SYMBOL(Driver_GPIO, DRV_GPIO);
static ARM_DRIVER_GPIO *drv =  &CREATE_SYMBOL(Driver_GPIO, DRV_GPIO);

// Global variables (used in this module only)
static volatile uint8_t         gpio_event;
static volatile ARM_GPIO_Pin_t  gpio_pin;
static volatile uint8_t         gpio_irq_cnt;

// Local functions
static void     GPIO_DrvEvent           (ARM_GPIO_Pin_t pin, uint32_t event);
static int32_t  PinUnderTestIsAvailable (void);
static void     PinUnderTestInit        (void);
static void     PinUnderTestUninit      (void);
static int32_t  AuxiliaryPinIsAvailable (void);
static void     AuxiliaryPinInit        (void);
static void     AuxiliaryPinUninit      (void);
static void     AuxiliaryPinConfigInput (void);
static void     AuxiliaryPinConfigOutput(void);
static void     AuxiliaryPinSetOutput   (uint32_t val);
static void     AuxiliaryPinDisable     (void);

// Helper functions

/*
  \fn            static void GPIO_DrvEvent (ARM_GPIO_Pin_t pin, uint32_t event)
  \brief         Store event(s) into a global variables.
  \detail        This is a callback function called by the driver upon an event(s).
  \param[in]     pin            GPIO pin
  \param[in]     event          GPIO event
  \return        none
*/
static void GPIO_DrvEvent (ARM_GPIO_Pin_t pin, uint32_t event) {
  gpio_event   |= event;
  gpio_pin      = pin;
  gpio_irq_cnt += 1U;
}

/*
  \fn            static int32_t PinUnderTestIsAvailable (void)
  \brief         Check if Pin Under Test is available.
  \detail        This function is used to skip executing a test if Pin Under Test is not available.
  \return        execution status
                   - EXIT_SUCCESS: Pin Under Test is available
                   - EXIT_FAILURE: Pin Under Test is not available
*/
static int32_t PinUnderTestIsAvailable (void) {

  if (drv->Setup(GPIO_PIN_UNDER_TEST, NULL) == ARM_DRIVER_OK) {
    return EXIT_SUCCESS;
  } else {
    TEST_MESSAGE("[FAILED] Pin Under Test is not available!");
    return EXIT_FAILURE;
  }
}

/*
  \fn            static void PinUnderTestInit (void)
  \brief         Initialize Pin Under Test.
  \param[in]     none
  \return        none
*/
static void PinUnderTestInit (void) {

  drv->Setup(GPIO_PIN_UNDER_TEST, NULL);
}

/*
  \fn            static void PinUnderTestUninit (void)
  \brief         Uninitialize Pin Under Test.
  \param[in]     none
  \return        none
*/
static void PinUnderTestUninit (void) {

  drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_INPUT);
  drv->Setup       (GPIO_PIN_UNDER_TEST, NULL);
}

/*
  \fn            static int32_t AuxiliaryPinIsAvailable (void)
  \brief         Check if Auxiliary Pin is available.
  \detail        This function is used to skip executing a test if Auxiliary Pin is not available.
  \return        execution status
                   - EXIT_SUCCESS: Auxiliary Pin is available
                   - EXIT_FAILURE: Auxiliary Pin is not available
*/
static int32_t AuxiliaryPinIsAvailable (void) {

  if (drv->Setup(GPIO_PIN_AUX, NULL) == ARM_DRIVER_OK) {
    return EXIT_SUCCESS;
  } else {
    TEST_MESSAGE("[FAILED] Auxiliary Pin is not available!");
    return EXIT_FAILURE;
  }
}

/*
  \fn            static void AuxiliaryPinInit (void)
  \brief         Initialize Auxiliary Pin.
  \param[in]     none
  \return        none
*/
static void AuxiliaryPinInit (void) {

  drv->Setup(GPIO_PIN_AUX, NULL);
}

/*
  \fn            static void AuxiliaryPinUninit (void)
  \brief         Uninitialize Auxiliary Pin.
  \param[in]     none
  \return        none
*/
static void AuxiliaryPinUninit (void) {

  drv->SetDirection(GPIO_PIN_AUX, ARM_GPIO_INPUT);
  drv->Setup       (GPIO_PIN_AUX, NULL);
}

/*
  \fn            static void AuxiliaryPinConfigInput (void)
  \brief         Configure Auxiliary Pin as Input.
  \param[in]     none
  \return        none
*/
static void AuxiliaryPinConfigInput (void) {

  drv->SetDirection(GPIO_PIN_AUX, ARM_GPIO_INPUT);
}

/*
  \fn            static void AuxiliaryPinConfigOutput (void)
  \brief         Configure Auxiliary Pin as Output with Push-pull Output mode.
  \param[in]     none
  \return        none
*/
static void AuxiliaryPinConfigOutput (void) {

  drv->SetOutputMode(GPIO_PIN_AUX, ARM_GPIO_PUSH_PULL);
  drv->SetDirection (GPIO_PIN_AUX, ARM_GPIO_OUTPUT);
}

/*
  \fn            static void AuxiliaryPinSetOutput (uint32_t val)
  \brief         Set Auxiliary Pin Output level (0 or 1).
  \param[in]     val            Output level
  \return        none
*/
static void AuxiliaryPinSetOutput (uint32_t val) {

  drv->SetPullResistor(GPIO_PIN_AUX, ARM_GPIO_PULL_NONE);
  drv->SetOutput      (GPIO_PIN_AUX, val);
}

/*
  \fn            static void AuxiliaryPinDisable (void)
  \brief         Auxiliary Pin Disable set Pin as Input with Open-drain Output mode.
  \param[in]     none
  \return        Intput level
*/
static void AuxiliaryPinDisable (void) {

  drv->SetOutput    (GPIO_PIN_AUX, 0U);
  drv->SetDirection (GPIO_PIN_AUX, ARM_GPIO_INPUT);
  drv->SetOutputMode(GPIO_PIN_AUX, ARM_GPIO_OPEN_DRAIN);
}

#endif                                  // End of exclude form the documentation

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\defgroup dv_gpio GPIO Validation
\brief GPIO driver validation
\details
The GPIO validation performs the following tests:
- API interface compliance
- Functions operation
- Event signaling

To perform GPIO validation tests, it is required to select and configure two pins in the <b>DV_GPIO_Config.h</b> configuration file:
- Pin Under Test: pin to be tested
- Auxiliary Pin: pin with serial low resistance resistor connected to Pin Under Test (suggested resistance of this resistor is around 1 kOhm)

\image html gpio_loopback.png

\note
 - Pins (Pin Under Test and Auxiliary Pin) should not have any external resistors or any external devices connected to it except the low resistance resistor used for testing.

\defgroup gpio_tests Tests
\ingroup dv_gpio
@{
*/

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/* GPIO tests                                                                                                               */
/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_Setup
\details
The function \b GPIO_Setup verifies the \b Setup function.
\code
  int32_t Setup (ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event);
\endcode
  
Testing sequence:
  - Call Setup function (without callback specified) and assert that it returned ARM_DRIVER_OK status
  - Call Setup function (with callback specified) and assert that it returned ARM_DRIVER_OK status
*/
void GPIO_Setup (void) {
  int32_t ret;

  if (PinUnderTestIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }

  // Call Setup function (without callback specified) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->Setup(GPIO_PIN_UNDER_TEST, NULL) == ARM_DRIVER_OK);

  // Call Setup function (with callback specified) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->Setup(GPIO_PIN_UNDER_TEST, GPIO_DrvEvent) == ARM_DRIVER_OK);

  PinUnderTestUninit();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetDirection
\details
The function \b GPIO_SetDirection verifies the \b SetDirection function.
\code
  int32_t ARM_GPIO_SetDirection (ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction);
\endcode

Testing sequence:
  - Call SetDirection function (with Input direction) and assert that it returned ARM_DRIVER_OK status
  - Configure Auxiliary Pin as Output
  - Drive Auxiliary Pin low
  - Read Pin Under Test Input level and assert that it returned 0
  - Drive Auxiliary Pin high
  - Read Pin Under Test Input level and assert that it returned 1
  - Configure Auxiliary Pin as Input
  - Call SetDirection function (with Output direction) and assert that it returned ARM_DRIVER_OK status
  - Call SetOutput function and set Output level low
  - Read Auxiliary Pin Input level and assert that it returned 0
  - Call SetOutput function and set Output level high
  - Read Auxiliary Pin Input level and assert that it returned 1
*/
void GPIO_SetDirection (void) {

  if (PinUnderTestIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }
  if (AuxiliaryPinIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }

  PinUnderTestInit();
  AuxiliaryPinInit();

  // Call SetDirection function (with Input direction) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_INPUT) == ARM_DRIVER_OK);

  // Configure Auxiliary Pin as Output
  AuxiliaryPinConfigOutput();

  // Drive Auxiliary Pin low
  AuxiliaryPinSetOutput(0U);

 (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 0U);

  // Drive Auxiliary Pin high
  AuxiliaryPinSetOutput(1U);

 (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 1U);

  // Configure Auxiliary Pin as Input
  AuxiliaryPinConfigInput();

  // Call SetDirection function (with Output direction) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_OUTPUT) == ARM_DRIVER_OK);

  // Call SetOutput function and set Output level low
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 0U);

 (void)osDelay(2U);

  // Read Auxiliary Pin Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_AUX) == 0U);

  // Call SetOutput function and set Output level high
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 1U);

 (void)osDelay(2U);

  // Read Auxiliary Pin Input level and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_AUX) == 1U);

  AuxiliaryPinUninit();
  PinUnderTestUninit();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetOutputMode
\details
The function \b GPIO_SetOutputMode verifies the \b SetOutputMode function.
\code
  int32_t ARM_GPIO_SetOutputMode (ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode);
\endcode

Testing sequence:
  - Call SetDirection function (with Output direction) and assert that it returned ARM_DRIVER_OK status
  - Call SetOutputMode function (with Push-pull mode) and assert that it returned ARM_DRIVER_OK status
  - Configure Auxiliary Pin as Input
  - Call SetOutput function and set Output level low
  - Read Auxiliary Pin Input level and assert that it returned 0
  - Call SetOutput function and set Output level high
  - Read Auxiliary Pin Input level and assert that it returned 1
  - Call SetOutputMode function (with Open-drain mode) and assert that it returned ARM_DRIVER_OK status
  - Call SetOutput function and set Output level low
  - Read Auxiliary Pin Input level and assert that it returned 0
*/
void GPIO_SetOutputMode (void) {

  if (PinUnderTestIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }
  if (AuxiliaryPinIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }

  PinUnderTestInit();
  AuxiliaryPinInit();

  // Call SetDirection function (with Output direction) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_OUTPUT) == ARM_DRIVER_OK);

  // Call SetOutputMode function (with Push-pull mode) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetOutputMode(GPIO_PIN_UNDER_TEST, ARM_GPIO_PUSH_PULL) == ARM_DRIVER_OK);

  // Configure Auxiliary Pin as Input
  AuxiliaryPinConfigInput();

  // Call SetOutput function and set Output level low
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 0U);

  (void)osDelay(2U);

  // Read Auxiliary Pin Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_AUX) == 0U);

  // Call SetOutput function and set Output level high
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 1U);

  (void)osDelay(2U);

  // Read Auxiliary Pin Input level and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_AUX) == 1U);

  // Call SetOutputMode function (with Open-drain mode) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetOutputMode(GPIO_PIN_UNDER_TEST, ARM_GPIO_OPEN_DRAIN) == ARM_DRIVER_OK);

  // Call SetOutput function and set Output level low
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 0U);

  (void)osDelay(2U);

  // Read Auxiliary Pin Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_AUX) == 0U);

  AuxiliaryPinUninit();
  PinUnderTestUninit();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetPullResistor
\details
The function \b GPIO_SetPullResistor verifies the \b SetPullResistor function.
\code
  int32_t ARM_GPIO_SetPullResistor (ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor);
\endcode

Testing sequence:
  - Call SetDirection function (with Input direction) and assert that it returned ARM_DRIVER_OK status
  - Call SetPullResistor function (with none resistor) and assert that it returned ARM_DRIVER_OK status
  - Configure Auxiliary Pin as Output
  - Drive Auxiliary Pin low
  - Read Pin Under Test Input level and assert that it returned 0
  - Drive Auxiliary Pin high
  - Read Pin Under Test Input level and assert that it returned 1
  - Disable Auxiliary Pin
  - Call SetPullResistor function (with Pull-down resistor) and assert that it returned ARM_DRIVER_OK status
  - Read Pin Under Test Input level and assert that it returned 0
  - Configure Auxiliary Pin as Output 
  - Drive Auxiliary Pin high
  - Read Pin Under Test Input level and assert that it returned 1
  - Disable Auxiliary Pin
  - Call SetPullResistor function (with Pull-up resistor) and assert that it returned ARM_DRIVER_OK status
  - Read Pin Under Test Input level and assert that it returned 1
  - Configure Auxiliary Pin as Output
  - Drive Auxiliary Pin low
  - Read Pin Under Test Input level and assert that it returned 0
*/
void GPIO_SetPullResistor (void) {

  if (PinUnderTestIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }
  if (AuxiliaryPinIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }

  PinUnderTestInit();
  AuxiliaryPinInit();

  // Call SetDirection function (with Input direction) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_INPUT) == ARM_DRIVER_OK);

  // Call SetPullResistor function (with none resistor) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_NONE) == ARM_DRIVER_OK);

  // Configure Auxiliary Pin as Output
  AuxiliaryPinConfigOutput();

  // Drive Auxiliary Pin low
  AuxiliaryPinSetOutput(0U);

  (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 0U);

  // Drive Auxiliary Pin high
  AuxiliaryPinSetOutput(1U);

  (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 1U);

  // Disable Auxiliary Pin
  AuxiliaryPinDisable();

  // Call SetPullResistor function (with Pull-down resistor) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_DOWN) == ARM_DRIVER_OK);

  (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 0U);

  // Configure Auxiliary Pin as Output
  AuxiliaryPinConfigOutput();

  // Drive Auxiliary Pin high
  AuxiliaryPinSetOutput(1U);

  (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 1U);

  // Disable Auxiliary Pin
  AuxiliaryPinDisable();

  // Call SetPullResistor function (with Pull-up resistor) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetPullResistor(GPIO_PIN_UNDER_TEST, ARM_GPIO_PULL_UP) == ARM_DRIVER_OK);

  (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 1U);

  // Configure Auxiliary Pin as Output
  AuxiliaryPinConfigOutput();

  // Drive Auxiliary Pin low
  AuxiliaryPinSetOutput(0U);

  (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 0U);

  AuxiliaryPinUninit();
  PinUnderTestUninit();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetEventTrigger
\details
The function \b GPIO_SetEventTrigger verifies the \b SetEventTrigger function.
\code
  int32_t ARM_GPIO_SetEventTrigger (ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger);
\endcode

Testing sequence:
  - Call Setup function (with callback specified) and assert that it returned ARM_DRIVER_OK status
  - Configure Auxiliary Pin as Output
  - Drive Auxiliary Pin low
  - Call SetEventTrigger function (with trigger on Rising-edge) and assert that it returned ARM_DRIVER_OK status
  - Trigger Rising-edge with drive Auxiliary Pin high
  - Call SetEventTrigger function (with trigger on Falling-edge) and assert that it returned ARM_DRIVER_OK status
  - Trigger Falling-edge with drive Auxiliary Pin low
  - Call SetEventTrigger function (with trigger on Either-edge) and assert that it returned ARM_DRIVER_OK status
  - Trigger Rising-edge with drive Auxiliary Pin high
  - Trigger Falling-edge with drive Auxiliary Pin low
  - Call SetEventTrigger function (with disabled triggering) and assert that it returned ARM_DRIVER_OK status
  - Trigger Rising/Falling-edge with drive Auxiliary Pin high/low
  - Call SetEventTrigger function (with trigger on Either-edge) and assert that it returned ARM_DRIVER_OK status
  - Disable IRQ
  - Trigger Rising/Falling-edge with drive Auxiliary Pin high/low
  - Enable IRQ
*/
void GPIO_SetEventTrigger (void) {

  if (PinUnderTestIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }
  if (AuxiliaryPinIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }

  PinUnderTestInit();
  AuxiliaryPinInit();

  // Call Setup function (with callback specified) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->Setup(GPIO_PIN_UNDER_TEST, GPIO_DrvEvent) == ARM_DRIVER_OK);

  // Configure Auxiliary Pin as Output
  AuxiliaryPinConfigOutput();

  // Drive Auxiliary Pin low
  AuxiliaryPinSetOutput(0U);

  gpio_event   = 0U;
  gpio_pin     = 0U;
  gpio_irq_cnt = 0U;

  // Call SetEventTrigger function (with trigger on Rising-edge) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_RISING_EDGE) == ARM_DRIVER_OK);

  // Trigger Rising-edge with drive Auxiliary Pin high
  AuxiliaryPinSetOutput(1U);

  (void)osDelay(2U);

  // Assert that event ARM_GPIO_EVENT_RISING_EDGE was signaled
  TEST_ASSERT_MESSAGE(gpio_event == ARM_GPIO_EVENT_RISING_EDGE, "[FAILED] Event ARM_GPIO_EVENT_RISING_EDGE was not signaled!");

  // Assert that pin GPIO_PIN_UNDER_TEST was triggered
  TEST_ASSERT_MESSAGE(gpio_pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  // Assert that number of interrupts was correct
  TEST_ASSERT_MESSAGE(gpio_irq_cnt == 1U, "[FAILED] Number of triggered interrupts is incorrect!");

  gpio_event   = 0U;
  gpio_pin     = 0U;
  gpio_irq_cnt = 0U;

  // Call SetEventTrigger function (with trigger on Falling-edge) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_FALLING_EDGE) == ARM_DRIVER_OK);

  // Trigger Falling-edge with drive Auxiliary Pin low
  AuxiliaryPinSetOutput(0U);

  (void)osDelay(2U);

  // Assert that event ARM_GPIO_EVENT_FALLING_EDGE was signaled
  TEST_ASSERT_MESSAGE(gpio_event == ARM_GPIO_EVENT_FALLING_EDGE, "[FAILED] Event ARM_GPIO_EVENT_FALLING_EDGE was not signaled!");

  // Assert that pin GPIO_PIN_UNDER_TEST was triggered
  TEST_ASSERT_MESSAGE(gpio_pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  // Assert that number of interrupts was correct
  TEST_ASSERT_MESSAGE(gpio_irq_cnt == 1U, "[FAILED] Number of triggered interrupts is incorrect!");

  gpio_event   = 0U;
  gpio_pin     = 0U;
  gpio_irq_cnt = 0U;

  // Call SetEventTrigger function (with trigger on Either-edge) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_EITHER_EDGE) == ARM_DRIVER_OK);

  // Trigger Rising-edge with drive Auxiliary Pin high
  AuxiliaryPinSetOutput(1U);

  (void)osDelay(2U);

  // Assert that event ARM_GPIO_EVENT_RISING_EDGE or ARM_GPIO_EVENT_EITHER_EDGE was signaled
  TEST_ASSERT_MESSAGE((gpio_event == ARM_GPIO_EVENT_RISING_EDGE) ||
                      (gpio_event == ARM_GPIO_EVENT_EITHER_EDGE), "[FAILED] Event ARM_GPIO_EVENT_RISING_EDGE was not signaled!");

  // Assert that pin GPIO_PIN_UNDER_TEST was triggered
  TEST_ASSERT_MESSAGE(gpio_pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  // Assert that number of interrupts was correct
  TEST_ASSERT_MESSAGE(gpio_irq_cnt == 1U, "[FAILED] Number of triggered interrupts is incorrect!");

  gpio_event   = 0U;
  gpio_pin     = 0U;
  gpio_irq_cnt = 0U;

  // Trigger Falling-edge with drive Auxiliary Pin low
  AuxiliaryPinSetOutput(0U);

  (void)osDelay(2U);

  // Assert that event ARM_GPIO_EVENT_FALLING_EDGE or ARM_GPIO_EVENT_EITHER_EDGE was signaled
  TEST_ASSERT_MESSAGE((gpio_event == ARM_GPIO_EVENT_FALLING_EDGE) ||
                      (gpio_event == ARM_GPIO_EVENT_EITHER_EDGE ), "[FAILED] Event ARM_GPIO_EVENT_FALLING_EDGE was not signaled!");

  // Assert that pin GPIO_PIN_UNDER_TEST was triggered
  TEST_ASSERT_MESSAGE(gpio_pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  // Assert that number of interrupts was correct
  TEST_ASSERT_MESSAGE(gpio_irq_cnt == 1U, "[FAILED] Number of triggered interrupts is incorrect!");

  gpio_event   = 0U;
  gpio_pin     = 0U;
  gpio_irq_cnt = 0U;

  // Call SetEventTrigger function (with disabled triggering) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_NONE) == ARM_DRIVER_OK);

  // Trigger Rising/Falling-edge with drive Auxiliary Pin high/low
  AuxiliaryPinSetOutput(1U);
  AuxiliaryPinSetOutput(0U);

  (void)osDelay(2U);

  // Assert that no event was triggered
  TEST_ASSERT_MESSAGE(gpio_event== 0U, "[FAILED] Event was triggered!");

  // Assert that no pin was triggered
  TEST_ASSERT_MESSAGE(gpio_pin == 0U, "[FAILED] Pin GPIO_PIN_UNDER_TEST was triggered!");

  // Assert that number of interrupts was correct 
  TEST_ASSERT_MESSAGE(gpio_irq_cnt == 0U, "[FAILED] Interrupt was triggered!");

  // Call SetEventTrigger function (with trigger on Either-edge) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetEventTrigger(GPIO_PIN_UNDER_TEST, ARM_GPIO_TRIGGER_EITHER_EDGE) == ARM_DRIVER_OK);

  gpio_event   = 0U;
  gpio_pin     = 0U;
  gpio_irq_cnt = 0U;

  __disable_irq();

  // Trigger Rising/Falling-edge with drive Auxiliary Pin high/low
  AuxiliaryPinSetOutput(1U);
  AuxiliaryPinSetOutput(0U);

  __enable_irq();

  (void)osDelay(2U);

  // Assert that event ARM_GPIO_EVENT_RISING_EDGE/ARM_GPIO_EVENT_FALLING_EDGE was signaled
  TEST_ASSERT_MESSAGE(gpio_event & (ARM_GPIO_EVENT_RISING_EDGE | ARM_GPIO_EVENT_FALLING_EDGE) ||
                     (gpio_event == ARM_GPIO_EVENT_EITHER_EDGE), "[FAILED] Both Event ARM_GPIO_EVENT_RISING_EDGE and ARM_GPIO_EVENT_FALLING_EDGE was not signaled!");

  // Assert that pin GPIO_PIN_UNDER_TEST was triggered
  TEST_ASSERT_MESSAGE(gpio_pin == GPIO_PIN_UNDER_TEST, "[FAILED] Pin GPIO_PIN_UNDER_TEST was not triggered!");

  // Assert that number of interrupts was correct
  TEST_ASSERT_MESSAGE((gpio_irq_cnt == 1U) || (gpio_irq_cnt == 2U), "[FAILED] Number of triggered interrupts is incorrect!");

  AuxiliaryPinUninit();
  PinUnderTestUninit();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetOutput
\details
The function \b GPIO_SetOutput verifies the \b GPIO_SetOutput function.

Testing sequence:
  - Call SetDirection function (with Output direction) and assert that it returned ARM_DRIVER_OK status
  - Call SetOutputMode function (with Push-pull mode) and assert that it returned ARM_DRIVER_OK status
  - Configure Auxiliary Pin as Input
  - Call SetOutput function and set Output level low
  - Read Auxiliary Pin Input level and assert that it returned 0
  - Call SetOutput function and set Output level high
  - Read Auxiliary Pin Input level and assert that it returned 1
*/
void GPIO_SetOutput (void) {
  if (PinUnderTestIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }
  if (AuxiliaryPinIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }

  PinUnderTestInit();
  AuxiliaryPinInit();

  // Call SetDirection function (with Output direction) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_OUTPUT) == ARM_DRIVER_OK);

  // Call SetOutputMode function (with Push-pull mode) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetOutputMode(GPIO_PIN_UNDER_TEST, ARM_GPIO_PUSH_PULL) == ARM_DRIVER_OK);

  // Configure Auxiliary Pin as Input
  AuxiliaryPinConfigInput();

  // Call SetOutput function and set Output level low
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 0U);

  (void)osDelay(2U);

  // Read Auxiliary Pin Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_AUX) == 0U);

  // Call SetOutput function and set Output level high
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 1U);

  (void)osDelay(2U);

  // Read Auxiliary Pin Input level and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_AUX) == 1U);

  AuxiliaryPinUninit();
  PinUnderTestUninit();
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_GetInput
\details
The function \b GPIO_GetInput verifies the \b GPIO_GetInput function.

Testing sequence:
  - Call SetDirection function (with Input direction) and assert that it returned ARM_DRIVER_OK status
  - Configure Auxiliary Pin as Output
  - Drive Auxiliary Pin low
  - Read Pin Under Test Input level and assert that it returned 0
  - Drive Auxiliary Pin high
  - Read Pin Under Test Input level and assert that it returned 1
*/
void GPIO_GetInput (void) {

  if (PinUnderTestIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }
  if (AuxiliaryPinIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }

  PinUnderTestInit();
  AuxiliaryPinInit();

  // Call SetDirection function (with Input direction) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_INPUT) == ARM_DRIVER_OK);

  // Configure Auxiliary Pin as Output
  AuxiliaryPinConfigOutput();

  // Drive Auxiliary Pin low
  AuxiliaryPinSetOutput(0U);

 (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 0U);

  // Drive Auxiliary Pin high
  AuxiliaryPinSetOutput(1U);

 (void)osDelay(2U);

  // Read Pin Under Test Input level and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 1U);

  AuxiliaryPinUninit();
  PinUnderTestUninit();
}

/**
@}
*/ 
// end of group dv_gpio
