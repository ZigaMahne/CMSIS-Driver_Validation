
# Task: Create or update a Device Under Test (DUT) project for CMSIS-Driver USART validation on the `<board_given_by_user>` board

## Prerequisites & Context

- [Reference pack documentation](https://github.com/ARM-software/CMSIS-Driver_Validation)
  - Template: portable project structure
  - Examples/USART: configuration and connection examples.
- Treat `<board_given_by_user>` as the DUT target.  
- The DUT project contains the USART driver tests and runs the CMSIS-Driver 
  Validation suite. A separate project runs a compatible USART Server on dedicated
  hardware to perform extensive communication tests.
- Establish the board/revision and MCU/core, DUT project/folder. Use available 
  documentation and project settings. Ask the user about essential missing information
  or unresolved configuration facts or multiple valid choices.
- Prepare the DUT project using an available CMSIS template, example project, or 
  board layer. Ensure the project includes the basic hardware configuration
 (startup, clock initialization, memory configuration), USART peripheral support
  and is ready to execute the validation application from provided app_main entry.
- Use CMSIS Developer Assistant throughout the entire workflow, following workspace
  instructions and relevant documentation.

## Execution Steps

### 1. Investigate the board and MCU:
   - Find the board manual/schematic, MCU datasheet/reference manual and errata.
   - Confirm board/revision and MCU/core. Identify available USART peripherals,
     MCU pins, connector pins, jumpers, solder bridges and conflicts.
   - Ask the user to choose if several valid routes remain.
   - If the USART Server project is unavailable in the workspace, ask the user for
     its path. If not provided, refer to the validation pack documentation,
     define the required signals and voltage levels, and mark server creation and
     connector mapping for later completion.
   - Document the planned DUT Tx -> Server Rx, DUT Rx <- Server Tx, common GND and
     any extra clock/modem/flow-control connections. Check documented I/O voltage
     compatibility and identify alternate wiring required by additional test modes.
     Identify any wiring changes required before enabling a different mode.
   - Provide relevant documentation and sections for the planned connections.
   - Result: Verified target identity, DUT pin assignments and a planned wiring
     table.

### 2. Select available CMSIS-Packs and board support:
   - Select mutually compatible released DFP, BSP and CMSIS-Driver_Validation pack.
     Prefer the latest releases within required existing constraints.
   - Verify that the selected packs support the identified board/revision, MCU/core.
   - Check the workspace first, then selected packs, to use available template,
     example and board layer for the required startup, system/clock initialization, 
     linker/memory configuration, and board initialization support.
     Use the available components to prepare the board layer.
     Use the add-board-layer skill when applicable.
   - Reuse the board layer from the pack without modification. Update it only when
     necessary to address issues found during validation.
   - Resolve CMSIS, CMSIS-RTX and CMSIS-Compiler dependencies.
   - Select the USART implementation and verify `Driver_USART<n>` against the physical
     peripheral and pins; driver and peripheral numbers need not match.
   - Result: Selected packs, a board layer and verified driver mapping.

### 3. Create or adapt DUT project:
   - For new projects, adapt the Driver Validation template, retaining only the
     USART context and selected board layer. Preserve the template filenames
     CMSIS_DV.csolution.yml and CMSIS_DV.cproject.yml.
     For existing projects, use the template to add missing validation components.
     Do not replace or duplicate app_main.
   - Verify that the DUT project includes the following components: 
     CMSIS Driver Validation Framework, CMSIS Driver Validation USART,
     CMSIS RTOS2 RTX5 and CMSIS OS Tick SysTick.
   - Check DUT data buffers requirements and allocate board layer startup/linker
     settings accordingly.
   - Configure the validation application to call cmsis_dv(NULL) once from an
     RTOS thread within the existing app_main flow, after board initialization.
   - Result: Configured USART validation context and resolved dependencies.

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

### 5. Configure USART validation:
   - Update the DV_USART_Config.h configuration file provided by the CMSIS Driver
     Validation USART component to match the DUT hardware and driver capabilities.
   - Set DRV_USART to the verified driver instance, USART_CFG_TEST_MODE to 1, and
     USART_CFG_SRV_MODE for asynchronous communication.
   - Specify matching server settings: asynchronous, 115200 baud, 8 data bits, no
     parity, 1 stop bit and no flow control.
   - Set data-exchange defaults separately to those same settings; use CPOL = 0 and
     CPHA = 0 as defaults for synchronous tests.
   - Set the minimum and maximum baud rates based on the DUT clock configuration,
     driver capabilities, documented USART Server support, and planned wiring.
   - Enable only features supported by the selected USART peripheral and board.
   - Check documented driver/server support. If server capabilities are not yet known,
     document the required capabilities and checks to complete before hardware testing.
   - Record excluded tests, reasons and required extra wiring/server support.
   - Result: Configured DV_USART_Config.h file and documented scope for later hardware testing.
   - See:
     [USART validation configuration documentation](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__usart__config.html)
     [DV_USART_Config.h](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Config/DV_USART_Config.h)

### 6. Verify DUT project:
   - Select the correct DUT solution and USART build context.
   - Build through CMSIS Developer Assistant, resolve errors and explain relevant warnings.
   - Check the resolved target, driver instance, USART Server test mode, RTOS
     integration and firmware/debugger semihosting configuration from project settings.
   - Record the context, build outcome and remaining limitations.
   - Result: A successfully built DUT project with supporting files; hardware
     execution and communication remain unverified.

### 7. Deliver concise project documentation:
   - Use the USART example README.md from the CMSIS-Driver_Validation pack as the
     reference:
     [USART example README.md](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Examples/USART/B-U585I-IOT02A/README.md)
   - Create or update the DUT README.md for `<board_given_by_user>` and the DUT
     configuration established in steps 1-6. Preserve the reference document's
     section headings, order and structure. Do not add new sections or information
     beyond these required updates.
   - Result: README.md adapted for `<board_given_by_user>`.
