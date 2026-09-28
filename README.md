# stm32-learning-06-clocks-and-timers
STM32 parctice code #05: clock and timer configuration and practice.

## HEAD detached at 45ffb93

### Description
验证 TIM IC indirect mode 能否正常工作。

PA8 输出一段高电平脉冲，由软件控制时长；PA10 使用 TIM1 的输入捕获功能获取脉冲长度。

实测可以正常工作。

### Hardware Connection
短接 PA10/PA8，连接 SWD。

### CubeMX Configuration
```text
HCLK = 72 MHz

TIM1:
Prescaler = 71  # 1 MHz
Counter: Mode = Up, Period = 65535.
Input Capture Channel 3: Rising Edge, Direct.
Input Capture Channel 4: Falling Edge, Indirect.
capture compare interrupt
```

Signal Path:
```text
PA10 -> TIM1_CH3
TIM1_CH3 -> TI3FP3 -> IC3
TIM1_CH3 -> TI3FP4 -> IC4
```

### Note
临时测试
