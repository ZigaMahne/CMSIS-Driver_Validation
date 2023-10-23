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
static uint32_t AuxiliaryPinGetInput    (void);
static void     AuxiliaryPinSetOutput   (uint32_t val);

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
  \brief         Configure Auxiliary Pin as Output.
  \param[in]     none
  \return        none
*/
static void AuxiliaryPinConfigOutput (void) {

  drv->SetOutputMode(GPIO_PIN_AUX, ARM_GPIO_PUSH_PULL);
  drv->SetDirection (GPIO_PIN_AUX, ARM_GPIO_OUTPUT);
}

/*
  \fn            static void AuxiliaryPinSetOutput (uint32_t val)
  \brief         Get Auxiliary Pin input level (0 or 1).
  \param[in]     val            Output level
  \return        none
*/
static uint32_t AuxiliaryPinGetInput (void) {

  return (drv->GetInput(GPIO_PIN_AUX));
}

/*
  \fn            static void AuxiliaryPinSetOutput (uint32_t val)
  \brief         Set Auxiliary Pin output level (0 or 1).
  \param[in]     val            Output level
  \return        none
*/
static void AuxiliaryPinSetOutput (uint32_t val) {

  drv->SetOutput(GPIO_PIN_AUX, val);
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
  - Call SetDirection function (with input direction) and assert that it returned ARM_DRIVER_OK status
  - Configure Auxiliary Pin as Output
  - Drive Auxiliary Pin low
  - Call GetInput function and assert that it returned 0
  - Drive Auxiliary Pin high
  - Call GetInput function and assert that it returned 1
  - Configure Auxiliary Pin as Input
  - Call SetDirection function (with output direction) and assert that it returned ARM_DRIVER_OK status
  - Call SetOutput function and set output level low
  - Read Auxiliary Pin input level and assert that it returned 0
  - Call SetOutput function and set output level high
  - Read Auxiliary Pin input level and assert that it returned 1
*/
void GPIO_SetDirection (void) {

  if (PinUnderTestIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }
  if (AuxiliaryPinIsAvailable() != EXIT_SUCCESS) { TEST_FAIL(); return; }

  PinUnderTestInit();                   // Initialize Pin Under Test
  AuxiliaryPinInit();                   // Initialize Auxiliary Pin

  // Call SetDirection function (with input direction) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_INPUT) == ARM_DRIVER_OK);

  AuxiliaryPinConfigOutput();           // Configure Auxiliary Pin as Output
  AuxiliaryPinSetOutput(0U);            // Drive Auxiliary Pin low

 (void)osDelay(2U);

  // Call GetInput function and assert that it returned 0
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 0U);

  AuxiliaryPinSetOutput(1U);            // Drive Auxiliary Pin high

 (void)osDelay(2U);

  // Call GetInput function and assert that it returned 1
  TEST_ASSERT(drv->GetInput(GPIO_PIN_UNDER_TEST) == 1U);

  AuxiliaryPinConfigInput();            // Configure Auxiliary Pin as Input

  // Call SetDirection function (with output direction) and assert that it returned ARM_DRIVER_OK status
  TEST_ASSERT(drv->SetDirection(GPIO_PIN_UNDER_TEST, ARM_GPIO_OUTPUT) == ARM_DRIVER_OK);

  // Call SetOutput function and set output level low
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 0U);

 (void)osDelay(2U);

  // Read Auxiliary Pin input level and assert that it returned 0
  TEST_ASSERT(AuxiliaryPinGetInput() == 0U);

  // Call SetOutput function and set output level high
  drv->SetOutput(GPIO_PIN_UNDER_TEST, 1U);

 (void)osDelay(2U);

  // Read Auxiliary Pin input level and assert that it returned 1
  TEST_ASSERT(AuxiliaryPinGetInput() == 1U);

  AuxiliaryPinUninit();                 // Uninitialize Auxiliary Pin
  PinUnderTestUninit();                 // Uninitialize Pin Under Test
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
*/
void GPIO_SetOutputMode (void) {
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
*/
void GPIO_SetPullResistor (void) {
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
*/
void GPIO_SetEventTrigger (void) {
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_SetOutput
\details
The function \b GPIO_SetOutput verifies the \b GPIO_SetOutput function.

Testing sequence:
*/
void GPIO_SetOutput (void) {
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
/**
\brief  Function: GPIO_GetInput
\details
The function \b GPIO_GetInput verifies the \b GPIO_GetInput function.

Testing sequence:
*/
void GPIO_GetInput (void) {
}

/**
@}
*/ 
// end of group dv_gpio
