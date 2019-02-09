# Controlling Nucleo LEDS via UART

Here is an attempt to control user LEDs from PC using UART
from [STM NUCLEO-F767ZI] development board.

Please read my [Getting started with ST NUCLEO F767ZI Board]
for development setup instructions.

> WARNING!
>
> It is work in progress...
>

## Source tree

The `UART_Printf/` is versioned as overlay (only changed files there)
of target directory:

```
STM32Cube_FW_F7_V1.14.0\Projects\STM32F767ZI-Nucleo\Examples\UART\UART_Printf\ 
```

For development you need to download and
extract `en.stm32cubef7.zip` from [STM32CubeF7]:

[STM NUCLEO-F767ZI]: https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-nucleo/nucleo-f767zi.html
[Getting started with ST NUCLEO F767ZI Board]: https://github.com/hpaluch/hpaluch.github.io/wiki/Getting-started-with-ST-NUCLEO-F767ZI-Board
[STM32CubeF7]: https://www.st.com/en/embedded-software/stm32cubef7.html
[STM32 Nucleo-144 boards]: https://www.st.com/content/ccc/resource/technical/document/user_manual/group0/26/49/90/2e/33/0d/4a/da/DM00244518/files/DM00244518.pdf/jcr:content/translations/en.DM00244518.pdf

