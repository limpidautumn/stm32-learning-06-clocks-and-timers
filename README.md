# stm32-learning-06-clocks-and-timers
STM32 parctice code #05: clock and timer configuration and practice.

## HEAD detached at 45ffb93

### Description
验证 TIM IC indirect mode 能否正常工作。

PA8 输出一段高电平脉冲，由软件控制时长；PA0 使用 TIM2 的输入捕获功能获取脉冲长度。

实测可以正常工作。

### Hardware Connection
短接 PA0/PA8，连接 SWD。

### CubeMX Configuration
```text
HCLK = 72 MHz

TIM2:
Prescaler = 71  # 1 MHz
Counter: Mode = Up, Period = 65535.
Input Capture Channel 1: Rising Edge, Direct.
Input Capture Channel 1: Falling Edge, Indirect.
global interrupt
```

Signal Path:
```text
PA0 -> TIM2_CH1
TIM2_CH1 -> TI1FP1 -> IC1
TIM2_CH1 -> TI1FP2 -> IC2
```

### Note
临时测试
