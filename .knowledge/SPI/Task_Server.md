
# Task: Create or update a Server project for CMSIS-Driver SPI validation on the `<board_given_by_user>` board

## Prerequisites & Context

- [Reference pack documentation](https://github.com/ARM-software/CMSIS-Driver_Validation)
  - Tools/SPI_Server/Template: portable project structure
  - Tools/SPI_Server/STM32F429I-DISC1: configuration and connection example
  - Examples/SPI: DUT configuration and connection examples
- Treat `<board_given_by_user>` as the SPI Server target.
- A separate Device Under Test (DUT) project runs the CMSIS-Driver SPI Validation
  tests and produces the validation report.
- Create or update a CMSIS SPI Server project that provides a physical test
  environment for the DUT. The Server supplies the required communication and
  signal behavior to perform extensive communication tests.
- Establish the board/revision and MCU/core, Server project/folder. Use available
  documentation and project settings. Ask the user about essential missing information,
  unresolved configuration facts or multiple valid choices.
- Prepare the Server project using an available CMSIS template, example project, or
  board layer. Ensure the project includes the basic hardware configuration
  (startup, clock initialization, memory configuration), SPI peripheral support
  and is ready to start the Server application from the provided app_main entry.
- Use CMSIS Developer Assistant throughout the workflow, following workspace
  instructions and relevant documentation.
- Keep Server specific changes within the Server project. Preserve unrelated DUT
  files, shared board layer and installed pack files.

## Execution Steps

### 1. Investigate the board and MCU:

   - Find the board manual/schematic, MCU datasheet/reference manual and errata.
   - Confirm board/revision and MCU/core. Identify available SPI peripherals,
     MCU pins, connector pins, jumpers, solder bridges and conflicts with the
     debug probe's Virtual COM Port, diagnostics and board functions.
   - Ask the user to choose if several valid routes remain.
   - If the SPI DUT project is unavailable in the workspace, ask the user for
     its path. If not provided, refer to the validation pack documentation,
     define the required signals and voltage levels and mark DUT connector mapping
     and compatibility checks for later completion.
   - Document the planned SCK-to-SCK, MOSI-to-MOSI, MISO-to-MISO, SS-to-SS and
     common GND connections, matching the paired DUT wiring table.
     For command exchange the DUT is master and the Server is slave. Check signal
     directions and SS ownership also for tests that reverse these roles.
   - Check documented I/O voltage compatibility and required level translation.
     Plan an external pull-up on SS to an appropriate compatible I/O supply
     (the reference uses approximately 10 kOhm at 3.3 V). Consider existing
     pull-ups and board topology.
   - Before executing SPI validation tests, ask the user to verify that the wiring
     is short, the ground connection is reliable, SCK is paired with GND where
     practical and other signal wires are kept separate.

   - Resolve conflicting pin assignments and output drivers before documenting
     connections. Identify any wiring changes required by additional test modes.
     Provide relevant documentation and sections for the planned connections.
   - Result: Verified target identity, Server pin assignments and a planned wiring
     table, with any missing DUT mapping and hardware checks recorded.

### 2. Select available CMSIS-Packs and board support:

   - Select mutually compatible released DFP, BSP and CMSIS-Driver_Validation packs.
     Prefer the latest releases within required existing constraints and verify
     compatibility with the DUT's validation release and command protocol.
   - Verify that the selected packs support the identified board/revision, MCU/core.
   - Check the workspace first, then selected packs, to use available template,
     example and board layer for the required startup, system/clock initialization,
     linker/memory configuration and board initialization support.
     Use the available components to prepare the board layer.
     Use the add-board-layer skill when applicable.
   - Reuse a compatible board layer without modifying installed pack or shared files.
     If adaptation is necessary, copy the layer into the Server project and keep
     board-specific changes and workarounds local to that project.
   - Resolve CMSIS, CMSIS-RTX and CMSIS-Compiler dependencies.
   - Select the SPI implementation and verify `Driver_SPI<n>` against the physical
     peripheral and pins, driver and peripheral numbers need not match. The Server
     and DUT driver instance numbers are independent.
   - Determine the SPI clock source and achievable master/slave bus speeds.
     Check documented driver and hardware support for the required master/slave
     roles, SS modes, clock/frame formats, data widths, bit orders, callbacks,
     transfers, counts, aborts and events. The Server must support slave operation
     with hardware-monitored SS for commands and the roles required by enabled tests.
     Compare these capabilities with the intended DUT tests and wiring.
     If DUT details are unavailable, record the required compatibility checks for later.
   - Result: Selected packs, a board layer and verified driver mapping, with a documented
     Server capability and wiring scope.

### 3. Create or adapt Server project:

   - For new projects, adapt the SPI Server template, retaining only the
     SPI context and selected board layer. Preserve the template filenames
     SPI_Server.csolution.yml and SPI_Server.cproject.yml.
     For existing projects, use the template to add missing Server components.
     Do not replace or duplicate app_main.
   - Verify that the Server project includes CMSIS RTOS2 RTX5 and CMSIS OS Tick SysTick
     components, with the selected SPI driver and device support and retains the Server layout:
     - app_main.c/.h
     - Config/SPI_Server_Config.h
     - Include/SPI_Server.h
     - Source/SPI_Server.c
     - RTE/CMSIS/RTX_Config.h and RTX_Config.c
     - Board/`<board_given_by_user>`
     - README.md
   - Keep a 1 kHz RTOS tick for unchanged reference timing. RTOS delays and flag-wait
     timeouts are expressed in ticks. Convert explicitly if a different tick
     frequency is required, consistently with the paired DUT's timing assumptions.
   - Check the Server's data buffers requirements and ensure the startup, linker
     and heap settings provide sufficient memory. Configure pins, clocks, interrupts
     and required DMA with RTOS-compatible priorities and DMA-accessible
     buffers/cache handling.
   - Review the driver's ability to switch DMA and peripheral widths between 8-bit
     commands and the enabled transfer widths. Check channel state before
     reconfiguration, preserve active transfers and record hardware verification
     for later testing.
   - Size the C runtime heap for both buffers, alignment and other allocations.
     The reference 4096-byte buffers require two 4100-byte allocations. Provide RTOS
     object memory and sufficient application and Server thread stacks, including
     the reference Server thread's 512-byte stack allocation.
   - Review startup code to ensure both allocation results are checked before use
     and both RX and TX buffers are initialized. Record actual allocation success as
     unverified until hardware execution.
   - Verify that the resolved CMSIS memory map matches the board and linker
     layout, explicitly declare any external flash/RAM missing from the packs and
     ensure each programmed flash region uses the correct programming algorithm.
   - Configure the Server application to call SPI_Server_Start() once from an
     RTOS thread within the existing app_main flow, after board initialization.
   - Preserve the template/example diagnostic approach with the board's supported
     VIO/STDIO initialization and retargeting. Keep diagnostics separate from the
     SPI test link and account for their effect on timing.
   - Result: Configured SPI Server context and resolved dependencies.

### 4. Configure SPI Server:

   - Update SPI_Server_Config.h configuration file to match the Server
     hardware, required coverage and timing capabilities:
     - SPI_SERVER_DRV_NUM set to the verified driver instance
     - SPI_SERVER_BUF_SIZE = 4096
     - SPI_SERVER_CMD_TIMEOUT = 100
   - Verify the fixed command settings in Source/SPI_Server.c:
     - Mode: Slave with hardware-monitored SS
     - Clock / Frame Format: CPOL = 0, CPHA = 0
     - Data Bits: 8
     - Bit Order: MSB first
   - Ensure the Server driver supports these fixed settings. For command exchange,
     the DUT generates SCK at SPI_CFG_SRV_BUS_SPEED. This speed must be supported
     by the Server in slave mode and by the wiring. Treat 1 MHz as a starting point
     only when supported.
   - Preserve the command protocol: 32-byte zero-padded commands, 16-byte GET VER
     and GET CNT responses and a 32-byte GET CAP response. Retain SET BUF, GET BUF,
     SET COM and XFER behavior. Buffer command lengths are bytes, XFER counts are
     data items.
   - SET COM selects transfer settings, XFER applies them temporarily and must
     restore the fixed slave command settings afterward.
   - Verify GET CAP against actual driver/hardware support. Control-based capability
     probing does not prove reliable transfers over board wiring.
   - Cross-check SPI_CFG_TEST_MODE = 1 and SPI_CFG_SRV_SS_MODE in the paired DUT,
     plus data-exchange defaults, minimum/maximum speeds, enabled widths, item counts
     and timeouts. Check SPI_CFG_SRV_CMD_TOUT and SPI_CFG_XFER_TIMEOUT against actual
     Server timing.
   - Do not equate SS-mode numeric encodings between DUT settings, Server source
     and command parameters. Preserve applicable tests for rejection of unsupported
     operations.
   - Result: Configured SPI_Server_Config.h file and documented scope consistent
     with the paired DUT task for later hardware testing.
   - See:
     [SPI Server documentation](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__spi__server.html)
     [SPI validation configuration](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__spi__config.html)
     [SPI validation and connections](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__dv__spi.html)

### 5. Review reference-Server portability and timing:

   - Review the selected Server source for the checks below. Apply only corrections
     required by that implementation, keep them in the Server project,
     preserve the wire protocol and document local changes.
   - Review GET CAP probing against supported clock, role and SS configurations.
     Check recovery after rejected Control calls and restoration of fixed command
     settings. Capability probing must not leave the Server driving signals that
     conflict with the DUT.
   - Check propagation of configuration, transfer and SS-control failures. Review
     completion/error flag handling, partial commands, timeouts and abort recovery.
   - Check that the SPI driver releases SS when the SPI interface is inactive and
     that role changes do not create conflicting SCK, MOSI, MISO or SS outputs.
     Preserve the external SS pull-up and required deactivation/settling guards.
   - Check the selected DUT sequence separately for each command.
   - For XFER, review delay_c, delay_t, the transfer window, SS activation and
     deactivation, restoration of command settings and the DUT's next-command delay.
     Include wire time, scheduling and diagnostics in the timing budget; preserve
     the reference's timing assumptions unless a verified change is required.
   - Increasing only the Server timeout does not extend the DUT deadline. Agree on
     changes needed at both ends when necessary.
   - Recheck the paired configuration after any portability or timing correction.
   - Result: Reviewed Server portability and timing, documented local changes and
     a list of remaining hardware checks.

### 6. Verify the project:

   - Select the correct Server solution and SPI build context.
   - Build through CMSIS Developer Assistant, resolve errors and explain relevant warnings.
   - Check the resolved target, Server driver instance and fixed command settings,
     RTOS integration, memory, board layer connections and configured diagnostics
     from project settings. Check DUT test mode and matching settings when available.
   - Document the paired hardware checks for later: Server startup, allocations,
     GET VER/GET CAP/GET CNT, transfers in both master/slave roles, maximum-size
     buffers and item counts, SS behavior, command-setting restoration and
     timeout/abort recovery. Do not report these as verified without a hardware run.
   - For later troubleshooting, if the Server does not respond, check startup,
     driver mapping and wiring, then consider a Server reset and a lower command
     speed. For "Server Start failed!", check both heap allocations, RTOS thread
     memory and support for the fixed slave/SS settings. For data mismatches,
     check SS pull-up, inactive-interface behavior, wiring and signal quality,
     and consider reducing bus speed.
   - Record the context, build outcome, output paths and remaining limitations.
     The DUT's semihosting file holds the validation report.
   - Result: A successfully built Server project with supporting files; hardware
     execution and communication remain unverified.

### 7. Deliver concise project documentation:

   - Use the SPI Server example README.md from the CMSIS-Driver_Validation pack as the
     reference:
     [SPI Server example README.md](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Tools/SPI_Server/STM32F429I-DISC1/README.md)
   - Create or update the Server README.md for `<board_given_by_user>` and the Server
     configuration established in steps 1-6. Adapt the board, wiring, command mode,
     supported features, diagnostics and build instructions to the actual project.
     Keep paired wiring, command settings and limitations consistent with the DUT.
   - Preserve the reference document's section headings, order and structure. Do not
     add new sections or information beyond these required updates.
   - Result: README.md adapted for `<board_given_by_user>`.
