
# Task: Create or update a Server project for CMSIS-Driver USART validation on the `<board_given_by_user>` board

## Prerequisites & Context

- [Reference pack documentation](https://github.com/ARM-software/CMSIS-Driver_Validation)
  - Tools/USART_Server/Template: portable project structure
  - Tools/USART_Server/STM32F429I-DISC1: configuration and connection example
  - Examples/USART: DUT configuration and connection examples
- Treat `<board_given_by_user>` as the USART Server target.
- A separate Device Under Test (DUT) project runs the CMSIS-Driver USART Validation
  tests and produces the validation report.
- Create or update a CMSIS USART Server project that provides a physical test
  environment for the DUT. The Server supplies the required communication and
  signal behavior to perform extensive communication tests.
- Establish the board/revision and MCU/core, Server project/folder. Use available
  documentation and project settings. Ask the user about essential missing information,
  unresolved configuration facts or multiple valid choices.
- Prepare the Server project using an available CMSIS template, example project, or
  board layer. Ensure the project includes the basic hardware configuration
  (startup, clock initialization, memory configuration), USART peripheral support
  and is ready to start the Server application from the provided app_main entry.
- Use CMSIS Developer Assistant throughout the entire workflow, following workspace
  instructions and relevant documentation.
- Keep Server specific changes within the Server project. Preserve unrelated DUT
  files, shared board layer and installed pack files.

## Execution Steps

### 1. Investigate the board and MCU:

   - Find the board manual/schematic, MCU datasheet/reference manual and errata.
   - Confirm board/revision and MCU/core. Identify available USART peripherals,
     MCU pins, connector pins, jumpers, solder bridges and conflicts with the
     debug probe's Virtual COM Port, diagnostics and board functions.
   - Ask the user to choose if several valid routes remain.
   - If the USART DUT project is unavailable in the workspace, ask the user for
     its path. If not provided, refer to the validation pack documentation,
     define the required signals and voltage levels and mark DUT connector mapping
     and compatibility checks for later completion.
   - Document the planned Server Tx -> DUT Rx, Server Rx <- DUT Tx, common GND and
     any extra clock/modem/flow-control connections. Check documented I/O voltage
     compatibility, required level translation and alternate wiring for additional
     test modes. Check complementary synchronous master/slave roles and the required
     clock, flow-control and modem signals. Identify any wiring changes required
     before enabling a different mode.
   - Resolve conflicting pin assignments before documenting connections.
   - Result: Verified target identity, Server pin assignments and a planned wiring
     table, mark any missing DUT mapping for later completion.

### 2. Select available CMSIS-Packs and board support:

   - Select mutually compatible released DFP, BSP and CMSIS-Driver_Validation packs.
     Prefer the latest releases within required existing constraints.
     Record the resolved pack, USART driver and compiler versions.
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
   - Select the USART implementation and verify `Driver_USART<n>` against the physical
     peripheral and pins; driver and peripheral numbers need not match.
   - Determine the actual USART clock source, frequency and oversampling
     configuration. Use these when assessing supported baud rates.
   - Check documented driver and hardware support for the required modes, formats,
     callbacks, transfers, counts/aborts and break/modem operations. Compare these
     with the intended DUT tests and wiring, document limitations and unknowns.
     If DUT details are unavailable, record the required compatibility checks for later.
   - Result: Selected packs, a board layer and verified driver mapping, with a documented
     Server capability and wiring scope.

### 3. Create or adapt Server project:

   - For new projects, adapt the Server template, retaining only the
     USART context and selected board layer. Preserve the template filenames
     USART_Server.csolution.yml and USART_Server.cproject.yml.
     For existing projects, use the template to add missing Server components.
     Do not replace or duplicate app_main.
   - Verify that the Server project includes CMSIS RTOS2 RTX5 and CMSIS OS Tick SysTick
     components and retains the relevant Server layout modules:
     - app_main.c/.h
     - Config/USART_Server_Config.h
     - Config/USART_Server_HW.c
     - Include/USART_Server.h and USART_Server_HW.h
     - Source/USART_Server.c
     - RTE/CMSIS/RTX_Config.h and RTX_Config.c
     - Board/`<board_given_by_user>`
     - README.md
   - Keep a 1 kHz RTOS tick for unchanged reference timing. RTOS delays and flag-wait
     timeouts are expressed in ticks. Convert explicitly if a different tick
     frequency is required.
   - Check the Server's data buffers requirements and ensure the startup, linker
     and heap settings provide sufficient memory. Configure pins, clocks, interrupts
     and required DMA with RTOS-compatible priorities and DMA-accessible
     buffers/cache handling.
   - If 9-bit DMA transfers are planned, review the driver's ability to switch between
     the widths required by 8-bit commands and 9-bit items. Check channel state before
     reconfiguration and preserve active transfers. Record hardware verification for
     later testing.
   - Size the C runtime heap for both buffers, alignment and other allocations.
     The reference 4096-byte buffers require two 4100-byte allocations. Provide RTOS
     object memory and sufficient application and Server thread stacks.
   - Review startup code to ensure both allocation results are checked before use
     and both RX and TX buffers are initialized. Record actual allocation success as
     unverified until hardware execution.
   - Verify that the resolved CMSIS memory map matches the board and linker
     layout, explicitly declare any external flash/RAM missing from the packs and
     ensure each programmed flash region uses the correct programming algorithm.
   - Configure the Server application to call USART_Server_Start() once from an
     RTOS thread within the existing app_main flow, after board initialization.
   - Preserve the template/example diagnostic approach with the board's supported
     VIO/STDIO initialization and retargeting. Keep diagnostics separate from the
     USART test link and account for their effect on timing.
   - Result: Configured USART Server context and resolved dependencies.

### 4. Configure USART Server:

   - Update the USART_Server_Config.h configuration file to match the Server
     hardware, required coverage and timing capabilities:
     - USART_SERVER_DRV_NUM set to the verified driver instance
     - USART_SERVER_MODE = 1
     - USART_SERVER_BUF_SIZE = 4096
     - USART_SERVER_CMD_TIMEOUT = 100
   - Implement USART_Server_HW.c module, including DCD/RI hooks with correct GPIO
     clocks, inactive startup levels and active-low signaling. If unused,
     retain the weak fallback or equivalent accurate capability handling,
     empty template overrides can falsely advertise support.
   - Keep command default server setting in in Source/USART_Server.c file:
     - Mode: Asynchronous
     - Data Bits: 8
     - Parity: None
     - Stop Bits: 1 stop bit
     - Flow control: No
     - Baudrate: 115200
   - Enable only features supported by the selected USART peripheral and board.
   - Preserve the command protocol: 32-byte zero-padded commands, a 16-byte GET VER
     response and a 32-byte GET CAP response.
     SET COM selects transfer settings, XFER must restore command settings afterward.
   - Verify GET CAP against actual driver/hardware support. Control-based format/baud
     probing does not prove reliable operation over board wiring. Validate the agreed
     baud limits and formats, account for two bytes per 9-bit item.
   - Check how the driver and HAL behave after rejected Control calls. Unsupported
     settings must not prevent subsequent valid configuration or communication.
   - Check DUT requirements: USART_CFG_TEST_MODE = 1, USART_CFG_SRV_MODE matching
     USART_SERVER_MODE, compatible command timing and tests supported by both ends
     and wiring.
   - Result: Configured USART_Server_Config.h file and documented scope for later
     hardware testing.
   - See:
     [USART Server documentation](https://arm-software.github.io/CMSIS-Driver_Validation/main/group__usart__server.html)

### 5. Review reference-Server portability and timing:

   - Review the selected Server source for the checks below. Apply only corrections
     required by that implementation, keep them in the Server project,
     preserve the wire protocol and document local changes.
   - Keep GET CAP baud probing within a hardware/driver-compatible range covering
     agreed requirements. Avoid blind extreme-rate probes: rejected settings can
     leave some HALs busy or disabled. Review recovery and record the hardware check.
   - Propagate configuration and RX/TX enable failures. Check restoration of command
     settings after GET CAP and XFER, plus recovery after incomplete commands,
     timeouts and aborts. Record runtime recovery as unverified until tested.
   - Check the selected DUT receive sequence separately for each command.
     Distinguish any wait before reception is armed from the receive timeout. The
     reply must start after receiver readiness and finish before the applicable
     deadline. Do not use one shortened delay for every command.
   - Increasing only the Server timeout does not extend the DUT deadline. Agree on
     changes needed at both ends when necessary. Otherwise document the timing
     requirements and confirm that replies fit during later hardware testing.
   - Result: Reviewed Server portability and timing, documented local changes and
     a list of remaining hardware checks.

### 6. Verify the project:

   - Select the correct Server solution and USART build context.
   - Build through CMSIS Developer Assistant, resolve errors and explain relevant warnings.
   - Check the resolved target, Server driver instance and command mode, RTOS
     integration, board layer connections and configured diagnostics from project
     settings. Check DUT test mode and matching settings only when the DUT is available.
   - Document the paired hardware checks for later: Server startup, allocations,
     GET VER/GET CAP, transfers both ways, maximum-length replies, buffers/counts,
     command-setting restoration and timeout/abort recovery. Do not report these
     as verified without a separate hardware run.
   - Record the context, build outcome, output paths and remaining limitations.
   - Result: A successfully built Server project with supporting files; hardware
     execution and communication remain unverified.

### 7. Deliver concise project documentation:

   - Use the USART Server example README.md from the CMSIS-Driver_Validation pack as the
     reference:
     [USART Server example README.md](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Tools/USART_Server/STM32F429I-DISC1/README.md)
   - Create or update the Server README.md for `<board_given_by_user>` and the Server
     configuration established in steps 1-6. Adapt the board, wiring, command mode,
     supported features, diagnostics and build instructions to the actual project.
   - Preserve the reference document's section headings, order and structure. Do not
     add new sections or information beyond these required updates.
   - Result: README.md adapted for `<board_given_by_user>`.
