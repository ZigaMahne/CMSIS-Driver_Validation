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
 * $Revision:   V1.0.0
 *
 * Project:     CMSIS-Driver Validation
 * Title:       General Input Output (GPIO) driver validation 
 *              configuration file
 *
 * -----------------------------------------------------------------------------
 */

#ifndef DV_GPIO_CONFIG_H_
#define DV_GPIO_CONFIG_H_

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h> GPIO
// <i> General Input Output (GPIO) driver validation configuration
// <o> Driver_GPIO# <2-255>
// <i> Choose the Driver_GPIO# instance to test pin
// <i> For example to test Driver_GPIO0 select 0
#define DRV_GPIO                         0
// <h> Configuration
// <o> Pin under test <0-160>
// <i> Select pin number setting to test
// <i> Pin need to be without external resistors on board
#define GPIO_PIN_UNDER_TEST              0
// <e> Additional Pins
#define GPIO_ADDITIONAL_PINS_EN          1
// <o> Pin with low resistor <0-160>
// <i> Select pin number with low resistor setting to test.
// <i> Lower resistor value (under 1k Ohm)
#define GPIO_PIN_LOWER                   0
// <o> Pin with high resistor  <0-160>
// <i> Select pin number with high resistor setting to test.
// <i> Higher resistor value (uper 1M Ohm)
#define GPIO_PIN_HIGHER                  0
// </e>
// </h>

// <h> Tests
// <i> Enable / disable tests
// <e> API
// <i> Enable / disable API tests.
#define GPIO_API_EN                      1
// <q> GPIO_Setup
// <i> Enable / disable Setup API test
#define GPIO_SETUP_EN                    1
// <q> GPIO_SetDirection
// <i> Enable / disable Direction API test
#define GPIO_SET_DIRECTION_EN            1
// <q> GPIO_SetOutputMode
// <i> Enable / disable OutputMode API test
#define GPIO_SET_OUTPUT_MODE_EN          1
// <q> GPIO_SetPullResistor
// <i> Enable / disable PullResistor Pull-Down/Up API test
#define GPIO_SET_PULL_RESISTOR_EN        1
// <q> GPIO_SetEventTrigger
// <i> Enable / disable EventTriggerEdge function test
#define GPIO_SET_EVENT_TRIGGER_EN        1
// </e>
// <e> Functional
// <i> On board connect Pin under test and selected additional Pins as Loopback
#define GPIO_FUNCTIONAL_EN               1
// <q> SetEventTrigger
// <i> Enable / disable EventTriggerEdge function test
#define SET_EVENT_TRIGGER_EN             1
// <q> SetOutput
// <i> Enable / disable Output level function test
#define SET_OUTPUT_EN                    1
// <q> GetInput
// <i> Enable / disable Input low level function test
#define GET_INPUT_EN                     1
// </e>
// </h>
// </h>

#endif /* DV_GPIO_CONFIG_H_ */
