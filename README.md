# stm32-learning-06-clocks-and-timers

<!-- This entire README was translated from Chinese by a large language model. -->

STM32 Practice Code #06: Clocks and Timers.

[English](./README.md) | [中文](./README.zh.md)

Understand the meaning and configuration of each part of the clock tree; use timers for precise timing-related input and output.

## Overview
This project is a hands-on exercise from [keysking's STM32 tutorial](https://space.bilibili.com/6100925/lists/1025423), running on [his development board](https://docs.keysking.com/docs/stm32/resourcePack/). I used STM32CubeMX and the HAL library to practice various timer usages.

In this project, I learned to use the [ETL library](https://github.com/ETLCPP/etl) and introduced some basic data structures. I implemented a simple task queue for non-blocking task scheduling. I also built a C++ abstraction layer for the STM32 timer's basic timing, input capture, output compare, PWM, and encoder modes to provide modular, object-oriented support.

### 1. Basic Timing
Commit [792bc60](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/792bc60cd0d002e61573ad44cd574d371894d272)

Poll and record the current TIM4 counter value every 100 ms, and send it over UART2.

1. Demonstrate the roles of the prescaler (PSC) and auto-reload register (ARR).
2. Use the internal clock source to demonstrate the timer's counting capability over time.

### 2. Update Interrupt
Commit [4c92152](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/4c921527214cd8e22e1edb95f8464e9422b9eb85)

Send a text string over UART2 every 1 s.

Demonstrate how to capture timer update interrupts.

### 3. External Clock Sources
- External clock mode 1, TIM2_CH2 input via TI2FP2 (Commit [9636375](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/96363751f6c544ea75be5bf8ba07ba0dd7ea0996))
- External clock mode 1, TIM2_CH1 input via TI1F_ED (Commit [e282a39](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/e282a393f1267098578dae6ac63077a174e5e685))
- External clock mode 1, TIM2_ETR input (Commit [02fe4ae](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/02fe4ae48466af96802b38926cd429bdf11de3be))
- External clock mode 2, TIM2_ETR input (Commit [4df6907](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/4df690748ea96efd93efef31ed6da61089fd5213))

Continuously send the current TIM2 counter value over UART2.

1. Understand the timer signal paths and related registers.
2. Demonstrate how to connect each external clock source.

In addition, testing showed that TIxFPx on the STM32F103C8T6 cannot detect both edges as a clock source.

### 4. Slave Modes
- Reset mode (Commit [5c974f7](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/5c974f73008b7e2796ac69fc47fa89e1da60aec0))
- Gated mode (Commit [55847ae](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/55847ae730080bc69a2ecacc8f8f9a653c2726f3))
- Trigger mode + One Pulse Mode (Commit [190aaa7](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/190aaa72ed2d43c6d293de98c8ae715c0c7e069a))
- External clock mode 1 is covered in Section 3; encoder mode is covered in Section 7.

TIM2_ETR (PA0) is used as the clock source, and TIM2_CH2 (PA1) controls slave-mode triggering. Continuously send the current TIM2 counter value over UART2.

1. Demonstrate the function of each slave mode.
2. Demonstrate capture of trigger interrupts.

### 5. Input Capture and Ultrasonic Ranging
Commit [5e72828](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/5e728283c0e464c4ceabb9c09ae5a548c7f18317)

Every 50 ms, call the HC_SR04 module to measure distance and capture the pulse width. It is state-machine driven with timeout and retry. Every 50 ms, calculate the distance from the previous result and send it over UART2.

External circuit: the HC_SR04 Trig pin is connected to PA11, and Echo is connected to PA10 (TIM1_CH3). Channel 3 captures the rising edge of TI3FP3, and Channel 4 captures the falling edge of TI3FP4.

1. Demonstrate the input capture function of a timer channel.
2. Demonstrate capture interrupt handling.

Note: To simplify the implementation, the speed of sound is assumed to be 343 m/s with no environmental compensation.

### 6. PWM Mode
Commit [4815624](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/4815624d79d1e8b449a0bf1caf431bbbcd451ddc)

A color LED continuously changes colors (red → yellow → green → cyan → blue → magenta → ...), with a 6 s cycle.

External circuit: the red LED is connected to PA6 (TIM3_CH1), green to PA7 (TIM3_CH2), and blue to PB0 (TIM3_CH3).

Demonstrate PWM generation mode on a timer channel.

Note: Output compare is implemented only as an abstraction layer and is not demonstrated separately.

### 7. Encoder Mode
Commit [a536b29](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/a536b290d14e2b0911ac9c1902b7f7a6a247b843)

Every 50 ms, read the current encoder value and send it over UART. TIM1 is configured in encoder mode and counts on TI1.

External circuit: phase A of the EC11 (incremental quadrature encoder) is connected to PA8 (TIM1_CH1), and phase B is connected to PA9 (TIM1_CH2).

Demonstrate the encoder slave-mode function.

For an improved reference, see Commit [dfd1230](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/dfd123055b1245ce307032c9259fe99a1181fa5e): optimized startup and 4x counting.

### 8. Practice: Driving a Motor with PWM

Commit [77d5453](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/77d545370ef21a73e4a25b015e79e0107b720714)

Every 5 ms, read the current encoder value and handle overflow, then:
1. **Servo**: use the encoder as input, output a PWM signal to drive the servo, and make the servo rotate in sync with the encoder.
2. **Open-loop DC motor**: when the button is pressed, use the encoder as input, output two PWM signals to drive the H-bridge, and control the DC motor's direction and speed in open loop; when the button is released, the motor stops.

External circuit:
1. Phase A of the EC11 encoder is connected to PA8 (TIM1_CH1), and phase B is connected to PA9 (TIM1_CH2).
2. The button is active-low and is connected to PB12. It is "KEY 1" on the board.
3. The SG90 servo control line is connected to PB8 (TIM4_CH3).
4. The DC motor's positive terminal is connected to DRV8833 AOUT1 and its negative terminal to AOUT2; DRV8833 AIN1 is connected to PA0 (TIM2_CH1), and AIN2 is connected to PA1 (TIM2_CH2).

Demonstrate the use of PWM signals.

## Code Architecture
<!-- Contains AI-generated content -->

The manually written application-layer code consists of `Core/Inc/*.hpp`, `Core/Src/*.cpp`, and `CMakeLists.txt`; `main.c` only calls `AppSetup()` / `AppLoop()` inside the `USER CODE` sections, and the remaining CubeMX-generated files are unmodified.

The abstraction layer lives in the `app` namespace:

- `CycCnt`: DWT cycle counter; `Enable()`, `cycles=Cyc(us)`, `cycle=Get()`, `Delay(cycles)`, `HasReached(cycle)`.
- `Pending`: delayed task queue; `Push(callback, delay_us)` enqueues a task, `Run()` executes due tasks in the main loop, and `Remove(callback)` cancels a task; the callback type is `etl::delegate<void()>`.
- `Error`: `Setup()` registers the exception handler, and `Fatal()` delays and then resets.
- `TimerBase`: `Setup()` starts all registered timers.
- `Timer<Channels...>`: pass `htim` and channel objects to the constructor, for example `Timer<TimerPwm> tim(&htim, ch);`; `SetPeriod(callback)` sets the update callback; `Counter()` / `SetCounter()` read and write the counter value.
- `TimerIc`: `SetCallback(callback)` sets the capture callback, and `Value()` reads the captured value.
- `TimerOc`: registration only.
- `TimerPwm`: `SetDuty(num, den)`; the duty cycle is `num / den`.
- `TimerEncoder`: pass `htim` to the constructor; access the count through the owning `Timer`'s `Counter()` / `SetCounter()`.

HAL callbacks are forwarded by `Core/Src/tim.cpp` to `TimerBase::DispatchPeriodElapsed()` and `TimerBase::DispatchCapture()`. Typical initialization sequence:

```cpp
CycCnt::Enable();
Error::Setup();
TimerBase::Setup();

while (true) Pending::Run();
```

## Related Links
### Tutorial Videos
[keysking](https://space.bilibili.com/6100925/) on Bilibili
1. [Clock Tree](https://www.bilibili.com/video/BV1ph4y1e7Ey/)
2. [Timers: Basic Timing](https://www.bilibili.com/video/BV11u4y1A7gS/)
3. [Timers: External Clock](https://www.bilibili.com/video/BV1N94y1u7Uz/)
4. [Timers: Slave Mode](https://www.bilibili.com/video/BV1mU421o7vt/)
5. [Timers: Input Capture](https://www.bilibili.com/video/BV1HM4m1R75B/)
6. [Timers: PWM](https://www.bilibili.com/video/BV1Yx4y1x7xY/)
7. [Timers: Encoder](https://www.bilibili.com/video/BV1f4421U7Uj/)
8. [PWM: Servo](https://www.bilibili.com/video/BV1mvpee4ECx/)
9. [PWM: Motor](https://www.bilibili.com/video/BV1jBxkeMEWU/)

### Personal Notes
For some of the more difficult topics, I was still confused after watching the videos, so I studied the reference manuals.

- [Clock tree](https://github.com/limpidautumn/learning-notes/blob/main/notes/stm32/clock-tree.md)
- [Timers](https://github.com/limpidautumn/learning-notes/blob/main/notes/stm32/timer.md)

### Reference Manuals and Datasheets
- [RM0008 - ST](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [DRV8833 - TI](https://www.ti.com/lit/ds/symlink/drv8833.pdf)
