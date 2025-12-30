# ESP32-PMBus-Reader

基于 [sxjack/dps750tb_psu_i2c](https://github.com/sxjack/dps750tb_psu_i2c) 项目，将其移植到 ESP32 平台（Arduino 框架）。

这是一个简单的 PMBus 电源监控器，通过 I2C 接口读取服务器电源（PSU）的实时遥测数据（电压、电流、功率、温度、转速），并通过串口打印输出。支持通过 PSON 引脚控制电源开关。

## 功能特性

*   **实时数据读取**：监测输入/输出电压 (V)、电流 (A)、功率 (W)。
*   **环境监控**：读取温度传感器数据及风扇转速 (RPM)。
*   **状态监控**：获取 PMBus 状态字。
*   **串口输出**：波特率 115200，方便调试或上位机开发。
*   **电源控制**：支持 PSON# 引脚控制电源启停。

## 硬件兼容性 / Tested Hardware

本项目代码已在以下电源型号上实测通过：

*   ✅ **LITEON PS-2461-7H**
*   Dell (Delta) DPS-750TB (参考原项目)

## 接线说明 / Wiring

默认引脚定义（可在代码中修改）：

| ESP32 引脚 | 功能 | 连接到 |
| :--- | :--- | :--- |
| GPIO 18 | I2C SDA | PSU SDA |
| GPIO 8 | I2C SCL | PSU SCL |
| GPIO 37 | PSON (输出) | PSU PSON# |
| GPIO 15 | I2C Enable (输出) | (视具体电路需求) |

> **注意**：服务器电源的 I2C 理论上已具有上拉电阻，如果上拉较弱，可能需要在 SDA 和 SCL 线上额外并联 2k-10k 的上拉电阻才能稳定通信。

## 如何使用

1.  使用 PlatformIO 打开项目 
2.  上传代码。
3.  打开串口监视器（波特率设为 115200）查看数据。

## 致谢

本项目基于以下开源项目修改：
*   [sxjack/dps750tb_psu_i2c](https://github.com/sxjack/dps750tb_psu_i2c) - 原始的 PMBus PSU I2C 读取实现

## 许可证

MIT License
