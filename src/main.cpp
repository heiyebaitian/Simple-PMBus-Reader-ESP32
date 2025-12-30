/*
 * Simple PMBus reader for ESP32 (Arduino)
 * Prints PSU data via Serial (115200 baud)
 */

#include <Arduino.h>
#include <Wire.h>
#include "pmbus.h"

// 定义引脚
const int PSON_do = 37;        // 控制 PSU 开关（可接 PSON#）
const int I2C_enable_do = 15;  // 使能 I2C （暂不清楚作用）
const uint8_t PMBUS_ADDR = 0x5F; // 默认地址为 0x5F（0x58~0x5F）

// 创建 PMBus
PMBus psu;

void setup() {
  Serial.begin(115200);
  while (!Serial); // 等待串口就绪

  pinMode(PSON_do, OUTPUT);
  pinMode(I2C_enable_do, OUTPUT);

  delay(100);

  // 初始化 I2C SCL=8, SDA=18
  Wire.begin(18, 8, 100000);

  // 初始化 PMBus
  psu.init(PSON_do, I2C_enable_do, 0, PMBUS_ADDR, &Serial, &Wire);

  // 启动电源（拉高 PSON#）
  psu.on();

  Serial.println("\n=== PMBus PSU Monitor (ESP32) ===");
  Serial.println("Reading data every second...\n");
}

void loop() {
  if (psu.scan()) { // 读取 PSU 数据
    Serial.printf("VIN: %.2f V\n", psu.V_in);
    Serial.printf("IIN: %.2f A\n", psu.I_in);
    Serial.printf("VOUT: %.2f V\n", psu.V_out);
    Serial.printf("IOUT: %.2f A\n", psu.I_out);
    Serial.printf("WOUT: %.1f W\n", psu.W_out);
    Serial.printf("Temp[0]: %.1f °C\n", psu.T[0]);
    Serial.printf("Temp[1]: %.1f °C\n", psu.T[1]);
    Serial.printf("Fan: %.0f RPM\n", psu.fan[0]);
    Serial.printf("Status Word: 0x%04X\n", psu.status_word);
    Serial.println("---");
  }

  delay(100);
}