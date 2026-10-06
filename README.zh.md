# stm32-learning-06-clocks-and-timers

STM32 练习代码 #06：时钟与定时器。

[English](./README.md) | [中文](./README.zh.md)

掌握时钟树各部分含义及配置方法；使用定时器完成时间精确的输入输出。

## 概述
本项目是 [keysking 的 STM32 教程](https://space.bilibili.com/6100925/lists/1025423) 的随堂练习，运行在 [他的开发板](https://docs.keysking.com/docs/stm32/resourcePack/) 上。我使用 STM32CubeMX 及 HAL 库，实践了定时器的各种用法。

在本项目中，我学习使用 [ETL 库](https://github.com/ETLCPP/etl)，引入了一些基础数据结构。我实现了一个简易的任务队列，用于实现非阻塞任务调度。我为 STM32 定时器的基础定时功能、输入捕获、输出比较、PWM 模式及编码器模式制作了 C++ 抽象层，以提供模块化和面向对象的支持。

### 一、基础定时
Commit [792bc60](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/792bc60cd0d002e61573ad44cd574d371894d272)

每隔 100ms 轮询记录当前 TIM4 定时器的计数值，并通过 UART2 发送。

1. 展示预分频器 (PSC) 及自动重装载寄存器 (ARR) 的作用。
2. 接入内部时钟源，展示定时器在时间上的计数功能。

### 二、更新中断
Commit [4c92152](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/4c921527214cd8e22e1edb95f8464e9422b9eb85)

每隔 1s 通过 UART2 发送一条文字内容。

展示定时器更新中断的捕获方法。

### 三、外部时钟源
- External clock mode 1, TIM2_CH2 通过 TI2FP2 作输入 (Commit [9636375](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/96363751f6c544ea75be5bf8ba07ba0dd7ea0996))
- External clock mode 1, TIM2_CH1 通过 TI1F_ED 作输入 (Commit [e282a39](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/e282a393f1267098578dae6ac63077a174e5e685))
- External clock mode 1, TIM2_ETR 作输入 (Commit [02fe4ae](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/02fe4ae48466af96802b38926cd429bdf11de3be))
- External clock mode 2, TIM2_ETR 作输入 (Commit [4df6907](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/4df690748ea96efd93efef31ed6da61089fd5213))

通过 UART2 持续发送当前 TIM2 的计数器值。

1. 了解定时器信号传递路径及相关寄存器。
2. 展示各外部时钟源的接入方法。

另外实测发现：STM32F103C8T6 上的 TIxFPx 不能检测双边沿作为时钟源。

### 四、从模式
- Reset mode (Commit [5c974f7](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/5c974f73008b7e2796ac69fc47fa89e1da60aec0))
- Gated mode (Commit [55847ae](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/55847ae730080bc69a2ecacc8f8f9a653c2726f3))
- Trigger mode + One Pulse Mode (Commit [190aaa7](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/190aaa72ed2d43c6d293de98c8ae715c0c7e069a))
- External mode 1 见第三节，Encoder mode 见第七节。

TIM2_ETR (PA0) 作时钟源，TIM2_CH2 (PA1) 控制从模式触发。通过 UART2 持续发送当前 TIM2 的计数器值。

1. 展示各个从模式的功能。
2. 展示触发中断的捕获。

### 五、输入捕获，超声波测距
Commit [5e72828](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/5e728283c0e464c4ceabb9c09ae5a548c7f18317)

每隔 50ms 调用 HC_SR04 模块进行测距，捕获脉冲宽度；状态机驱动，超时重试。每隔 50ms 根据上一次的结果计算出距离，并通过 UART2 发送。

外部电路：HC_SR04 模块的 Trig 连接到 PA11，Echo 连接到 PA10 (TIM1_CH3)。Channel 3 捕获 TI3FP3 的上升沿，Channel 4 捕获 TI3FP4 的下降沿。

1. 展示定时器通道的输入捕获功能。
2. 展示“捕获中断”的捕获。

另：为简化实现，假定声速为 343m/s，没有环境补偿。

### 六、PWM 模式
Commit [4815624](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/4815624d79d1e8b449a0bf1caf431bbbcd451ddc)

彩色 LED 灯连续变换颜色（红→黄→绿→青→蓝→品→...），6s 一次循环。

外部电路：红色 LED 连接 PA6 (TIM3_CH1)，绿色连接 PA7 (TIM3_CH2)，蓝色连接 PB0 (TIM3_CH3)。

展示定时器通道的 PWM 生成模式。

另：输出比较仅实现抽象层，未单独演示。

### 七、编码器模式
Commit [a536b29](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/a536b290d14e2b0911ac9c1902b7f7a6a247b843)

每隔 50ms 读取编码器当前读数，通过 UART 发送。TIM1 配置为编码器模式，TI1 计数。

外部电路：EC11 (增量式正交编码器) 的A相连接到 PA8 (TIM1_CH1)，B相连接到 PA9 (TIM1_CH2)。

展示编码器从模式的功能。

改进参考 Commit [dfd1230](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/dfd123055b1245ce307032c9259fe99a1181fa5e)：优化启动、四倍频计数。

### 八、实践：PWM 驱动电机

Commit [77d5453](https://github.com/limpidautumn/stm32-learning-06-clocks-and-timers/tree/77d545370ef21a73e4a25b015e79e0107b720714)

每隔 5ms，读取编码器当前读数并做溢出处理，接着：
1. **舵机**：将编码器作为输入，输出 PWM 信号驱动舵机，舵机跟随编码器同步转动。
2. **直流电机开环**：按下按键后，将编码器作为输入，输出两路 PWM 信号驱动 H 桥，开环控制直流电机的转向及转速；松开按键，电机停转。

外部电路：
1. EC11 编码器的A相连接到 PA8 (TIM1_CH1)，B相连接到 PA9 (TIM1_CH2)。
2. 按键按下输出低电平，连接到 PB12。板上为“KEY 1”。
3. SG90 舵机的控制线连接到 PB8 (TIM4_CH3)。
4. 直流电机的正极连接 DRV8833 的 AOUT1 引脚，负极连接 AOUT2；DRV8833 的 AIN1 连接 PA0 (TIM2_CH1)，AIN2 连接 PA1 (TIM2_CH2)。

展示 PWM 信号的用途。

## 代码架构
<!-- 含 AI 生成内容 -->

应用层手工代码为 `Core/Inc/*.hpp`、`Core/Src/*.cpp` 及 `CMakeLists.txt`；`main.c` 只在 `USER CODE` 区域调用 `AppSetup()` / `AppLoop()`，其余 CubeMX 生成文件未修改。

抽象层均位于 `app` 命名空间：

- `CycCnt`：DWT 周期计数器；`Enable()`、`cycles=Cyc(us)`、`cycle=Get()`、`Delay(cycles)`、`HasReached(cycle)`。
- `Pending`：延迟任务队列；`Push(callback, delay_us)` 入队，`Run()` 在主循环执行到期任务，`Remove(callback)` 取消任务；回调类型为 `etl::delegate<void()>`。
- `Error`：`Setup()` 注册异常处理，`Fatal()` 延时后复位。
- `TimerBase`：`Setup()` 启动所有已注册定时器。
- `Timer<Channels...>`：构造时传入 `htim` 和通道对象，例如 `Timer<TimerPwm> tim(&htim, ch);`；`SetPeriod(callback)` 设置更新回调；`Counter()` / `SetCounter()` 读写计数值。
- `TimerIc`：`SetCallback(callback)` 设置捕获回调，`Value()` 读取捕获值。
- `TimerOc`：仅注册。
- `TimerPwm`：`SetDuty(num, den)`，占空比为 `num / den`。
- `TimerEncoder`：构造时传入 `htim`，通过所属 `Timer` 的 `Counter()` / `SetCounter()` 访问计数。

HAL 回调由 `Core/Src/tim.cpp` 转发到 `TimerBase::DispatchPeriodElapsed()` 和 `TimerBase::DispatchCapture()`。典型初始化顺序：

```cpp
CycCnt::Enable();
Error::Setup();
TimerBase::Setup();

while (true) Pending::Run();
```

## 相关链接
### 教程原视频
[keysking](https://space.bilibili.com/6100925/) on BiliBili
1. [时钟树](https://www.bilibili.com/video/BV1ph4y1e7Ey/)
2. [定时器: 基础定时](https://www.bilibili.com/video/BV11u4y1A7gS/)
3. [定时器: 外部时钟](https://www.bilibili.com/video/BV1N94y1u7Uz/)
4. [定时器: 从模式](https://www.bilibili.com/video/BV1mU421o7vt/)
5. [定时器: 输入捕获](https://www.bilibili.com/video/BV1HM4m1R75B/)
6. [定时器: PWM](https://www.bilibili.com/video/BV1Yx4y1x7xY/)
7. [定时器: 编码器](https://www.bilibili.com/video/BV1f4421U7Uj/)
8. [PWM: 舵机](https://www.bilibili.com/video/BV1mvpee4ECx/)
9. [PWM: 电机](https://www.bilibili.com/video/BV1jBxkeMEWU/)

### 个人笔记
部分比较难的知识点，我看完视频之后还是有些迷惑，于是我去啃手册了。

- [时钟树](https://github.com/limpidautumn/learning-notes/blob/main/notes/stm32/clock-tree.md)
- [定时器](https://github.com/limpidautumn/learning-notes/blob/main/notes/stm32/timer.md)

### 数据手册
- [RM0008 - ST](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [DRV8833 - TI](https://www.ti.com/lit/ds/symlink/drv8833.pdf)
