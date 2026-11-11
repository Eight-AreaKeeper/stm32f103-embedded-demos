# STM32F103 Embedded Demos

Embedded C demos for the **STM32F103C8T6**, built while working through an online STM32 course (江协科技 / Jiangxie Technology). All projects use the **STM32F10x Standard Peripheral Library** in **Keil MDK**.

Each folder is a self-contained Keil project (`*.uvprojx`) with the usual layout: `User/` (main.c), `Hardware/` (module drivers), `System/` (delay / timer helpers), `Library/` (Standard Peripheral Library), `Start/` (startup and CMSIS files).

## Demos

| Folder | What it does | Peripherals |
|---|---|---|
| `2-1 STM32` | First project: configures PC13 as push-pull output and drives the on-board LED | GPIO |
| `3-1Project_LED` | Blinks an LED on PA0 using `GPIO_ResetBits/SetBits` and `GPIO_WriteBit` | GPIO |
| `3-2Project_LED` | LED chaser ("running light") across PA0–PA7 using `GPIO_Write` | GPIO |
| `3-3Project_BUZZER` | Toggles a buzzer on PB12 every 500 ms | GPIO |
| `3-4Project_BUTTON_LED` | Two buttons (PB1, PB11, input pull-up) turn an LED on/off | GPIO (input + output) |
| `3-5Project_LIGHTSENSOR_BUZZER` | Light sensor module digital output (PB13) switches a buzzer on/off | GPIO |
| `4-1Project_OLED` | OLED driver test: shows strings, decimal, signed, hex and binary numbers | GPIO (software I2C on PB8/PB9) |
| `5-1Iinfrared_sensor` | Sets up an external interrupt on PB14 for an infrared counting sensor; OLED shows text | EXTI, AFIO, GPIO |
| `5-2Rotary_ENCODER` | Reads a rotary encoder via external interrupts on PB0/PB1 and shows the running count on OLED | EXTI, AFIO, GPIO |
| `6-1TIMER` | TIM2 update interrupt once per second increments a counter shown on OLED, along with the live CNT value | TIM (internal clock, interrupt), NVIC |
| `6-2TIMER_EXTERNAL_CLOCK` | TIM2 clocked from external pulses on PA0 (ETR mode 2); interrupt every 10 pulses, count shown on OLED | TIM (external clock), NVIC |
| `6-3PWM_LED` | "Breathing" LED: ramps TIM2 CH1 PWM duty on PA0 up and down | TIM, PWM |
| `6-4PWM_SERVO` | Servo control: each button press adds 30° (wraps after 180°), angle shown on OLED | TIM4 PWM (PB8), GPIO |
| `6-6IPUT_CAPTURE` | Generates a 1 kHz PWM on PA0 and measures its frequency with input capture on PA6 | TIM2 PWM, TIM3 input capture |
| `6-7PWMI_DUTYCYCLE` | Same setup in PWM-input mode: measures both frequency and duty cycle on PA6 | TIM2 PWM, TIM3 PWMI |
| `6-8编码器接口测速` | Encoder speed measurement: TIM3 encoder interface (PA6/PA7) read once per second by a TIM2 interrupt | TIM encoder interface, TIM interrupt |
| `7-1AD_CHANNEL` | Single-channel ADC on PA0: shows raw value (0–4095) and converted voltage | ADC1, GPIO (analog) |
| `7-2AD_MULTICHANNEL` | Reads 4 ADC channels (PA0–PA3: potentiometer, light sensor, thermistor, reflective IR) and shows all on OLED | ADC1, GPIO (analog) |
| `8-1DMA_DATATRANSFER` | Work in progress: contains a DMA1 Channel1 driver (`MyDMA.c`); `main.c` is still the OLED template | DMA |
| `X-1Project_OLED_test` | Extra: frame-buffer OLED animation of a dancing gorilla with a floating music note | GPIO (software I2C OLED) |

## Hardware & Tools

- STM32F103C8T6 board ("Blue Pill" style)
- Keil MDK-ARM, µVision 5
- STM32F10x Standard Peripheral Library
- ST-Link programmer/debugger
- Modules used in the code: 0.96" OLED (I2C, software-driven), LEDs, active buzzer, push buttons, light sensor module, infrared counting sensor, rotary encoder, servo motor, potentiometer, thermistor, reflective IR sensor

## Build

1. Open the `.uvprojx` file in a demo folder with Keil µVision 5.
2. Build the project (F7).
3. Connect the board with an ST-Link and flash it (Download, F8).
