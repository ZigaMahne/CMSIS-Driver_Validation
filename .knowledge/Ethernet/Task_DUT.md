
# Task: Create or update a Device Under Test (DUT) project for CMSIS-Driver Ethernet validation on the `<board_given_by_user>` board

## Prerequisites & Context

- [Reference pack documentation](https://github.com/ARM-software/CMSIS-Driver_Validation)
  - Template: portable project structure
  - Examples/Ethernet: configuration and loopback example
- Treat `<board_given_by_user>` as the DUT target.
- The DUT project contains the Ethernet MAC and PHY drivers tests and runs the
  CMSIS-Driver Validation suite using internal loopback and an external Ethernet
  loopback plug. These tests do not require a separate validation Server.
- Establish the board/revision and MCU/core, DUT project/folder and Ethernet
  hardware. Use available documentation and project settings. Ask the user about
  essential missing information, unresolved configuration facts or multiple valid choices.
- Prepare the DUT project using an available CMSIS template, example project, or 
  board layer. Ensure the project includes the basic hardware configuration
  (startup, clock initialization, memory configuration), Ethernet MAC/PHY support
  and is ready to execute the validation application from provided app_main entry.
- Use CMSIS Developer Assistant throughout the entire workflow, following workspace
  instructions and relevant documentation.

## Execution Steps

### 1. Investigate the board and MCU:
   - Find the board manual/schematic, MCU datasheet/reference manual, PHY datasheet
     and relevant errata through the CMSIS documentation tools.
   - Confirm board/revision and MCU/core. Identify the Ethernet MAC, PHY model,
     connector, media interface, MCU pins, jumpers, solder bridges and conflicts.
   - Identify MII/RMII data and control signals, MDIO/MDC, PHY management address
     and straps, reset/power control and reference-clock source and direction.
     Verify required clock frequency and startup sequence for the selected interface.
     Check that the selected PHY supports this loopback arrangement and the suite's
     operating modes.
   - Ask the user to choose if several valid Ethernet interfaces or routes remain.
   - Provide relevant documentation and sections for the planned connections.
   - Result: Verified target, MAC/PHY and clock identities, with planned loopback
     connections and any unresolved hardware requirements recorded.

### 2. Select available CMSIS-Packs and board support:
   - Select mutually compatible released DFP, BSP, CMSIS-Driver and CMSIS-Driver_Validation
     pack. Prefer the latest releases within required existing constraints.
   - Verify support for the identified board/revision, MCU/core and PHY model.
     Resolve CMSIS, CMSIS-RTX and CMSIS-Compiler dependencies.
   - Check the workspace first, then selected packs, for an available template,
     example and board layer providing startup, system/clock initialization,
     linker/memory configuration and board initialization support.
     Use the available components to prepare the board layer.
     Use the add-board-layer skill when applicable.
   - Reuse the board layer from the pack without modification. Update it only when
     necessary to address issues found during validation.
   - Resolve CMSIS, CMSIS-RTX and CMSIS-Compiler dependencies.
   - Select the Ethernet MAC and PHY implementations and verify their exported
     `Driver_ETH_MAC<n>` and `Driver_ETH_PHY<n>` symbols against the physical pair.
     The validation suite's DRV_ETH selector chooses both symbols with the same
     suffix. It is not the PHY's MDIO management address or necessarily the MCU
     peripheral number. Resolve mismatches using documented driver configuration.
   - Verify that the MAC provides PHY_Read/PHY_Write callbacks compatible with the
     selected PHY driver. Check media-interface selection, management address,
     clocks and board initialization against the driver configuration.
   - Result: Selected packs, a board layer and verified MAC/PHY driver mapping.

### 3. Create or adapt DUT project:
   - For new projects, adapt the Driver Validation template, retaining only the
     Ethernet context and selected board layer. Preserve the template filenames
     CMSIS_DV.csolution.yml and CMSIS_DV.cproject.yml.
     For existing projects, use the template to add missing validation components.
     Do not replace or duplicate app_main.
   - Verify that the DUT project includes the following components:
     CMSIS Driver Validation Framework, CMSIS Driver Validation Ethernet,
     CMSIS RTOS2 RTX5 and CMSIS OS Tick SysTick, with the selected Ethernet MAC,
     Ethernet PHY and device support. Use component identifiers from the selected packs.
   - Check Ethernet pins, clocks, interrupts, DMA configuration and C library heap.
     Verify descriptor and buffer placement in DMA-accessible memory,
     required alignment and cache/MPU handling.
   - Check validation timing and RTOS tick frequency to use a 1 ms RTX tick.
     Preserve this timebase unless the selected implementation supports another rate.
   - Configure the validation application to call cmsis_dv(NULL) once from an RTOS
     thread within the existing app_main flow, after board initialization. Ensure
     another network stack or application thread does not concurrently own the MAC/PHY.
   - Result: Configured Ethernet validation context and resolved dependencies.

### 4. Update DUT project to use semihosting:
   - In the DUT project, convert or add STDIO to semihosting for the selected
     compiler/runtime. Apply the required changes across all affected board layer files.
   - If exist, remove the UART STDIO retargeting module that implements stdio_init,
     stderr_putchar, stdout_putchar and stdin_getchar functions.
     Do not leave a stale UART STDOUT provider.
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

### 5. Configure Ethernet validation:
   - Update the DV_ETH_Config.h configuration file provided by the CMSIS Driver
     Validation Ethernet component to match the DUT hardware and driver capabilities.
   - Set DRV_ETH to the verified common MAC/PHY driver instance.
   - Set ETH_LINK_TIMEOUT (default 3000 ms) and ETH_TRANSFER_TIMEOUT (default 100 ms)
     against expected PHY startup and transfer behavior.
     Set ETH_PTP_TIME_REF (default 1000 ms) and ETH_PTP_TOLERANCE (default 0 ns).
     Choose any tolerance from documented clock accuracy and timing requirements.
   - Review tests against the selected suite and driver capabilities.
   - Review Loopback tests separately. The suite exercises MAC internal loopback,
     the external test also checks PHY internal loopback, then negotiates the physical
     link and transfers through the plug.
   - Check the selected test source's speed/duplex assumptions. The reference configures
     100 Mbit/s full duplex in several tests.
   - Result: Configured DV_ETH_Config.h file and documented coverage, timing assumptions,
     exclusions and loopback requirements for later hardware testing.
   - See:
     [Ethernet validation and connections](https://arm-software.github.io/CMSIS-Driver_Validation/latest/group__dv__eth.html)
     [Ethernet validation configuration documentation](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__eth__config.html)
     [DV_ETH_Config.h](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Config/DV_ETH_Config.h)
     [Ethernet validation implementation](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Source/DV_ETH.c)

### 6. Verify DUT project:
   - Select the correct DUT solution and Ethernet build context.
   - Build through CMSIS Developer Assistant, resolve errors and explain relevant
     warnings.
   - Check the resolved target, MAC/PHY instance, media interface, PHY management
     address, clock/reset configuration, RTOS timing, heap/stack requirements
     and firmware/debugger semihosting settings. Check MAC/PHY configuration,
     RX events or polling, DMA descriptors/buffers, cache handling and timeouts.
   - Record the context, pack versions, build outcome, test scope and limitations.
   - Result: A successfully built DUT project with supporting files.
     Hardware execution and communication remain unverified.

### 7. Deliver concise project documentation:
   - Use the Ethernet example README.md from the CMSIS-Driver_Validation pack as the
     reference:
     [Ethernet example README.md](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Examples/Ethernet/NUCLEO-H563ZI/README.md)
   - Create or update the DUT README.md for `<board_given_by_user>` and the DUT
     configuration established in steps 1-6. Preserve the reference document's
     section headings, order and structure. Do not add new sections or information
     beyond these required updates.
   - Result: README.md adapted for `<board_given_by_user>`.
