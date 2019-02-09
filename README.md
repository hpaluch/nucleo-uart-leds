# Controlling Nucleo LEDS via UART

Here is an attempt to control user LEDs from PC using UART
from [STM NUCLEO-F767ZI] development board.

Please read my [Getting started with ST NUCLEO F767ZI Board]
for development setup instructions.

> Following commands are implemented (from `help`):
>
> ```
> help      - to show this help
> led1 on   - to turn green LED LD1 on
> led1 off  - to turn green LED LD1 off
> led2 on   - to turn blue  LED LD2 on
> led2 off  - to turn blue  LED LD2 off
> ```

## Source tree

The `UART_Printf/` is versioned as overlay (only changed files there)
of target directory:

```
STM32Cube_FW_F7_V1.14.0\Projects\STM32F767ZI-Nucleo\Examples\UART\UART_Printf\ 
```

For development you need to download and
extract `en.stm32cubef7.zip` from [STM32CubeF7]:

# Accessing UART from Windows

At first you need to know correct serial port
name - `COMx` (where `x` is some number).

To get it:
* open `Device Manager` (for example running `devmgmt.msc`)
* Expand `Ports (COM & LPT)`
* There should be entry
  like `STMicroelectronics STLink Virtual COM Port COM5`

In my case name of UART (serial port) is `COM5`.

Now we need some program to access UART.
There is mentioned `Hyperterminal` in original ST docs but it is
no longer included in Windows.

However we can use [Putty] for this purpose.

To configure Putty for UART access:
* run Putty
* enter suitable name into "Saved Sessions:" for example `STM32-9600-7bit-odd`
* select "Connection type:" `Serial`
* on Category tree expand `Connection` -> `Serial`
* ensure that following values are configured:
  - "Serial line connect to": your COM port name, for example `COM5`
  - "Speed (baud):" `9600`
  - "Data bits:" `7`
  - "Stop bits:" `1`
  - "Parity:" `Odd`
  - "Flow Control:" `None`
* click back on `Session` in "Category:" tree
* click on `Save`

Now you can just double-click on session name `STM32-9600-7bit-odd`
to open connection to UART (serial port).

If you try original `STM32F767ZI-Nucleo\Examples\UART\UART_Printf\`
example from STM you should see after each reset on Nucleo board text like:

```
 UART Printf Example: retarget the C library printf function to the UART
** Test finished successfully. **

```

# Known bugs/limitations

* Command editing (arrows, delete, backspace) does not work
  and produce confusing results...



[STM NUCLEO-F767ZI]: https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-nucleo/nucleo-f767zi.html
[Getting started with ST NUCLEO F767ZI Board]: https://github.com/hpaluch/hpaluch.github.io/wiki/Getting-started-with-ST-NUCLEO-F767ZI-Board
[STM32CubeF7]: https://www.st.com/en/embedded-software/stm32cubef7.html
[STM32 Nucleo-144 boards]: https://www.st.com/content/ccc/resource/technical/document/user_manual/group0/26/49/90/2e/33/0d/4a/da/DM00244518/files/DM00244518.pdf/jcr:content/translations/en.DM00244518.pdf
[Putty]: https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html

