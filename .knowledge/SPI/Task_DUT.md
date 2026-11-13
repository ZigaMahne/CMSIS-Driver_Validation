
# Task: Create or update a Device Under Test (DUT) project for CMSIS-Driver SPI validation on the `<board_given_by_user>` board

## Prerequisites & Context

- [Reference pack documentation](https://github.com/ARM-software/CMSIS-Driver_Validation)
  - Template: portable project structure
  - Examples/SPI: configuration and connection example
  - Tools/SPI_Server: Server template and example
- Treat `<board_given_by_user>` as the DUT target.
- The DUT project contains the SPI driver tests and runs the CMSIS-Driver
  Validation suite. A separate project runs a compatible SPI Server on dedicated
  hardware to perform extensive communication tests.
- Establish the board/revision and MCU/core, DUT project/folder. Use available
  documentation and project settings. Ask the user about essential missing
  information, unresolved configuration facts or multiple valid choices.
- Prepare the DUT project using an available CMSIS template, example project, or
  board layer. Ensure the project includes the basic hardware configuration
  (startup, clock initialization, memory configuration), SPI peripheral support
  and is ready to execute the validation application from the provided app_main entry.
- Use CMSIS Developer Assistant throughout the workflow, following workspace
  instructions and relevant documentation.

## Execution Steps

### 1. Investigate the board and MCU:
   - Find the board manual/schematic, MCU datasheet/reference manual and errata.
   - Confirm board/revision and MCU/core. Identify available SPI peripherals,
     MCU pins, connector pins, jumpers, solder bridges and conflicts with other
     board functions.
   - Ask the user to choose if several valid routes remain.
   - If the SPI Server project is unavailable in the workspace, ask the user for
     its path. If not provided, refer to the validation pack documentation,
     define the required signals and voltage levels and mark Server creation,
     connector mapping and compatibility checks for later completion.
   - Document the planned SCK-to-SCK, MOSI-to-MOSI, MISO-to-MISO, SS-to-SS and
     common GND connections. For command exchange the DUT is master and the
     Server is slave. Check the signal directions and SS ownership also for
     tests that reverse these roles. Avoid conflicting outputs.
   - Check documented I/O voltage compatibility and any required level translation.
     Plan an external pull-up on SS to an appropriate compatible I/O supply
     (the reference uses approximately 10 kOhm at 3.3 V). Consider existing
     pull-ups and board topology.
   - Provide relevant documentation and sections for the planned connections.
   - Result: Verified target identity, DUT pin assignments and a planned wiring
     table, with any missing Server mapping and hardware checks recorded.

### 2. Select available CMSIS-Packs and board support:
   - Select mutually compatible released DFP, BSP and CMSIS-Driver_Validation packs.
     Prefer the latest releases within required existing constraints.
   - Verify that the selected packs support the identified board/revision, MCU/core.
   - Check the workspace first, then selected packs, to use an available template,
     example and board layer for startup, system/clock initialization, linker/memory
     configuration and board initialization support.
     Use the available components to prepare the board layer.
     Use the add-board-layer skill when applicable.
   - Reuse the board layer from the pack without modification. Update it only when
     necessary to address issues found during validation.
   - Resolve CMSIS, CMSIS-RTX and CMSIS-Compiler dependencies.
   - Select the SPI implementation and verify `Driver_SPI<n>` against the physical
     peripheral and pins; driver and peripheral numbers need not match. Determine
     the SPI clock source and achievable bus speeds.
   - Check documented DUT driver and peripheral support for master/slave operation,
     SS modes, formats, data widths, bit orders, transfers and events. Compare these
     with the intended Server, wiring and test coverage; record unknowns.
   - Result: Selected packs, a board layer and verified driver mapping.

### 3. Create or adapt DUT project:
   - For new projects, adapt the Driver Validation template, retaining only the
     SPI context and selected board layer. Preserve the template filenames
     CMSIS_DV.csolution.yml and CMSIS_DV.cproject.yml.
     For existing projects, use the template to add missing validation components.
     Do not replace or duplicate app_main.
   - Verify that the DUT project includes the following components:
     CMSIS Driver Validation Framework, CMSIS Driver Validation SPI,
     CMSIS RTOS2 RTX5 and CMSIS OS Tick SysTick, with the selected SPI driver
     and device support.
   - Check SPI pins, clocks, interrupts and any required DMA. Check RTOS
     interrupt priorities, DMA-accessible memory, alignment and cache handling
     where applicable. Avoid duplicate startup or peripheral initialization.
   - Check the selected validation implementation's buffer, heap and stack
     requirements. Check Server buffer capacity against the planned transfers when
     its configuration is available.
   - Check the validation timing and RTOS tick frequency. Preserve its timing
     assumptions or convert delays and timeouts consistently when a different
     tick frequency is required.
   - Configure the validation application to call cmsis_dv(NULL) once from an
     RTOS thread within the existing app_main flow, after board initialization.
   - Result: Configured SPI validation context and resolved dependencies.

### 4. Update DUT project to use semihosting:
   - In the DUT project, convert or add STDIO to semihosting for the selected
     compiler/runtime. Apply the required changes across all affected board layer files.
   - If exist, remove the UART STDIO retargeting module that implements stdio_init,
     stderr_putchar, stdout_putchar and stdin_getchar functions.
   - Remove stdio_init() calls from application initialization code.
   - Remove its associated CMSIS-Compiler:STDOUT/STDIN/STDERR component references.
     Do not invent a "Semihosting" component.
     Note: removing these components may leave unresolved permitted validation dependencies.
   - In the DUT csolution.yml file, configure settings for a debug adapter.
     For a supported pyOCD debugger, add or update:
   ```yaml
     debugger:
       name: <actual supported adapter>@pyOCD
       protocol: swd
       stdio:
         - mode: file
           file-out: <report-path>
   ```
   - Document the intended report location for further service semihosting during
     later testing.
   - Result: Board layer STDIO converted to semihosting, UART retargeting removed
     and document the intended report location for later semihosting output capture.
   - See: [pyOCD Debugger STDIO documentation](https://open-cmsis-pack.github.io/cmsis-toolbox/pyOCD-Debugger/#stdio)

### 5. Configure SPI validation:
   - Update the DV_SPI_Config.h configuration file provided by the selected CMSIS
     Driver Validation SPI component to match the DUT hardware and driver capabilities.
   - Set DRV_SPI to the verified driver instance and SPI_CFG_TEST_MODE to 1 for
     SPI Server testing. Configure SPI_CFG_SRV_SS_MODE for a supported software-
     or hardware-controlled DUT SS output and SPI_CFG_SRV_BUS_SPEED for reliable
     command exchange with the selected Server.
   - Verify the Server's fixed command settings: the DUT is master; the Server is
     slave with hardware-monitored SS; clock polarity 0 and phase 0, 8 data bits,
     and MSB first. The Server driver must support these settings. The command bus
     speed is generated by the DUT and must be supported by both ends and wiring.
   - Configure the data-exchange defaults separately: a supported SS mode,
     CPOL = 0, CPHA = 0, 8 data bits, MSB first and a suitable bus speed and item
     count. Treat the template's 1 MHz as a starting point only when supported.
   - Set minimum and maximum test bus speeds based on the DUT clock, driver,
     documented Server capabilities and planned wiring. Check the actual achievable
     rates and record any coverage restricted by the Server or test setup.
   - Configure item counts, data-width mask, command and transfer timeouts,
     memory capacity and the supported features of both ends. Consider master/slave
     and SS modes, clock and frame formats, data widths, bit order, transfers, aborts
     and events. Retain applicable tests for correct rejection of unsupported operations.
   - Check documented driver/Server support and the required role transitions and
     SS behavior. If Server capabilities are not yet known, record the required
     checks and unresolved settings before hardware testing.
   - Record excluded tests, reasons and any required extra wiring or Server support.
     Loopback is an optional preliminary test with MOSI connected to MISO, with
     limited coverage and does not replace Server-mode validation.
   - Result: Configured DV_SPI_Config.h file and documented scope for later hardware testing.
   - See:
     [SPI validation configuration documentation](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__spi__config.html)
     [SPI validation and connections](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__dv__spi.html)
     [SPI Server documentation](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__spi__server.html)
     [DV_SPI_Config.h](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Config/DV_SPI_Config.h)

### 6. Verify DUT project:
   - Select the correct DUT solution and SPI build context.
   - Build through CMSIS Developer Assistant, resolve errors and explain relevant warnings.
   - Check the resolved target, driver instance, SPI Server test mode, command
     settings, RTOS integration, memory requirements and firmware/debugger
     semihosting configuration from project settings.
   - Record the context, build outcome, selected test scope and remaining limitations.
     Document later paired checks for wiring, SS pull-up, Server startup, command
     communication.
     For later troubleshooting, check the driver mapping, Server readiness and
     command speed if the Server does not respond. For data mismatches, check
     SS pull-up, wiring and signal quality and consider reducing the bus speed.
   - Result: A successfully built DUT project with supporting files; hardware
     execution and communication remain unverified.

### 7. Deliver concise project documentation:
   - Use the SPI example README.md from the selected CMSIS-Driver_Validation pack
     as the reference:
     [SPI example README.md](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Examples/SPI/B-U585I-IOT02A/README.md)
   - Create or update the DUT README.md for `<board_given_by_user>` and the DUT
     configuration established in steps 1-6. Preserve the reference document's
     section headings, order and structure. Do not add new sections or information
     beyond these required updates.
   - Result: README.md adapted for `<board_given_by_user>`.
