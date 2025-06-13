# CoreMark Benchmark - FRDM-MCXA153

## What is CoreMark?

**CoreMark** is a benchmark developed by **EEMBC (Embedded Microprocessor Benchmark Consortium)** to measure the performance of CPUs in embedded systems. It targets operations commonly found in real embedded workloads, such as:

- Linked list manipulation
- Matrix operations
- Integer math
- CRC (cyclic redundancy check)

CoreMark is designed to be portable, consistent, and reliable for comparing embedded processor performance.

---

## About this project

This project implements the CoreMark benchmark on the **FRDM-MCXA153** development board (NXP MCX A153, ARM Cortex-M33 core). In addition to running the benchmark once, the application has been extended to:

- **Periodically execute the CoreMark benchmark**, using a configurable interval defined by the macro `EXECUTION_TIME_FREQ` (in seconds).
- Use the **LPTMR (Low-Power Timer)** to schedule benchmark executions at runtime without blocking the main application.
- **Read the internal temperature** of the microcontroller at the time of each benchmark run.
- **Print benchmark results and temperature via serial terminal**.
- **Store each result in flash memory** for later inspection or logging.

---

## Requirements

- **FRDM-MCXA153** development board
- Tools:
  - [MCUXpresso IDE](https://www.nxp.com/mcuxpresso/ide)
  - USB drivers and debug probe support
- MCX A153 SDK (via NXP’s SDK Builder)
- Micro-USB cable
- Serial terminal software (e.g., PuTTY, Tera Term)

---

## Supported Boards
- [FRDM-MCXA153](../../_boards/frdmmcxa153/demo_apps/hello_world/example_board_readme.md)

---

### View the benchmark results

1. Open a serial terminal and connect to the correct COM port:
   - Baud rate: **115200**
   - 8 data bits, no parity, 1 stop bit (8N1)
2. Reset the board or press the reset button.
3. You will see CoreMark benchmark output, including:
   - Total execution time
   - Iterations per second
   - CRC values (to verify validity)

---

### Configure the execution interval

The interval between CoreMark executions is set via the `EXECUTION_TIME_FREQ` macro:

#define EXECUTION_TIME_FREQ 60  // Execute every 60 seconds

---

### Example Output

```
MCXA153 Coremark.
Ejecutando
Current temperature: 26.168

Starting CoreMark.
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 46890277
Total time (secs): 46
Iterations/Sec   : 61
Iterations       : 2850
Compiler version : GCC13.2.1 20231009
Compiler flags   : Low optimization
Memory location  : STACK
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0x25e3
Correct operation validated. See readme.txt for run and reporting rules.
Coremark execution completed!
Current temperature: 26.600
Prueba finalizada
```

---

## Flash Memory Output and Data Logging

For additional information on how benchmark data is stored or accessed from **flash memory**, please refer to the accompanying document:  
📄 [`README.pdf`](README.pdf)

This PDF contains implementation notes and instructions for retrieving performance data directly from non-volatile memory if needed.

---

## License

CoreMark is developed and maintained by EEMBC. For full licensing details and compliance, visit [https://www.eembc.org/coremark](https://www.eembc.org/coremark).

---
## Authors
Sergio Ramírez Preciado

Héctor Miguel Hernández Vargas
