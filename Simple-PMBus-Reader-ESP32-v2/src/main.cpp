/*
  CSPS 电源控制器（串口版）
  功能：
  1. 串口打印电源信息
  2. 串口控制电源 / 风扇

  参考串口命令如下
  MCU
  INFO
  Power_SET 1
  Power_SET 0
  SET_FAN 20000
  SET_REPORT 1000

*/


#define Version_v "CSPS-Serial-V1.0"

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "CSPS.h"

/* ---------- IO 定义 ---------- */
#define POW_ON              14  // MOS 管控制
#define POW_KEY_ON          12  // 电源按键
#define Insertion_Detection 16  // 电源插入检测
#define CSPS_SCL 8  // 电源I2C SCL
#define CSPS_SDA 18 // 电源I2C SDA

/* ---------- EEPROM 地址 ---------- */
#define SET_REPORT_addr  10
#define SET_FAN_addr     20
#define SET_AC_ON_addr   30

// 全局变量
unsigned int Powe_STATE = 0;
unsigned int Insertion_STATE = 0;

int EEPROM_loops_delay = 0;
int EEPROM_FAN = 0;
int EEPROM_SET_AC_ON = 0;

unsigned long previousTime = 0;


// 函数声明

void init_io();
void init_eeprom();

void Serial_CMD_Loop();
void INFO_Loop();
void KEY_Loop();

void Power_SET(int cmd);
void SET_FAN(int val);
void SET_AC_ON(int val);
void SET_REPORT(int val);

String GET_INFO();
String GET_Power_INFO();

int analysis_Serial_to_int(String in, String key);
void test_csps();





// 初始化 IO
void init_io() {
  pinMode(POW_ON, OUTPUT);
  pinMode(POW_KEY_ON, INPUT_PULLUP);
  pinMode(Insertion_Detection, INPUT_PULLUP);

  digitalWrite(POW_ON, LOW);
  delay(500);

  Wire.begin(CSPS_SDA, CSPS_SCL, 100000);

  if (digitalRead(Insertion_Detection) == LOW) {
    Insertion_STATE = 1;
    //Wire.begin(18, 8, 100000);
    delay(1000);
  }
}

// 初始化 EEPROM
void init_eeprom() {
  EEPROM.begin(512);

  EEPROM.get(SET_REPORT_addr, EEPROM_loops_delay);
  EEPROM.get(SET_FAN_addr, EEPROM_FAN);
  EEPROM.get(SET_AC_ON_addr, EEPROM_SET_AC_ON);

  if (EEPROM_SET_AC_ON == 1) {
    Power_SET(1);
    delay(1000);
    CSPS_SET_Fan(EEPROM_FAN);
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("CSPS Serial Start");

  init_io();
  init_eeprom();
}

void loop() {
  Serial_CMD_Loop();
  INFO_Loop();
  KEY_Loop();
}

// 按键控制
void KEY_Loop() {
  if (digitalRead(POW_KEY_ON) == LOW) {
    Power_SET(1);
    delay(1000);
    CSPS_SET_Fan(EEPROM_FAN);
  }
}

// 定时打印电源信息
void INFO_Loop() {
  if (EEPROM_loops_delay == 0) return;

  unsigned long now = millis();
  if (now - previousTime > EEPROM_loops_delay) {
    previousTime = now;
    Serial.println(GET_Power_INFO());
  }
}

// 串口命令
void Serial_CMD_Loop() {
  if (!Serial.available()) return;

  String cmd;
  while (Serial.available()) {
    cmd += char(Serial.read());
    delay(2);
  }

  cmd.trim();

  if (cmd.indexOf("MCU") != -1) {
    Serial.println(Version_v);
  }
  else if (cmd.indexOf("INFO") != -1) {
    Serial.println(GET_INFO());
  }
  else if (cmd.indexOf("Power_SET") != -1) {
    Power_SET(analysis_Serial_to_int(cmd, "Power_SET"));
  }
  else if (cmd.indexOf("SET_FAN") != -1) {
    SET_FAN(analysis_Serial_to_int(cmd, "SET_FAN"));
  }
  else if (cmd.indexOf("SET_AC_ON") != -1) {
    SET_AC_ON(analysis_Serial_to_int(cmd, "SET_AC_ON"));
  }
  else if (cmd.indexOf("SET_REPORT") != -1) {
    SET_REPORT(analysis_Serial_to_int(cmd, "SET_REPORT"));
  }
  else if (cmd.indexOf("TEST") != -1) {
    test_csps();
  }
}

// 功能函数
void Power_SET(int cmd) {
  if (cmd == 1) {
    digitalWrite(POW_ON, HIGH);
    Powe_STATE = 1;
  } else {
    digitalWrite(POW_ON, LOW);
    Powe_STATE = 0;
  }
}

void SET_FAN(int val) {
  EEPROM_FAN = val;
  CSPS_SET_Fan(val);
  EEPROM.put(SET_FAN_addr, val);
  EEPROM.commit();
}

void SET_AC_ON(int val) {
  EEPROM_SET_AC_ON = val;
  EEPROM.put(SET_AC_ON_addr, val);
  EEPROM.commit();
}

void SET_REPORT(int val) {
  EEPROM_loops_delay = val;
  EEPROM.put(SET_REPORT_addr, val);
  EEPROM.commit();
}

// 信息

// 这里屏蔽了电源插入检测，如有需要请移除下方代码注释
String GET_INFO() {
  // if (!Insertion_STATE)
  //   return "{\"ERROR\":\"未插入电源\"}";

  return "{\"OEM\":\"" + CSPS_GET_manufacturer() +
         "\",\"PN\":\"" + CSPS_GET_productName() + "\"}";
}

String GET_Power_INFO() {
  // if (!Insertion_STATE)
  //   return "{\"ERROR\":\"未插入电源\"}";

  return "{"
    "\"Powe_STATE\":" + String(Powe_STATE) + ","
    "\"IN_V\":" + String(CSPS_IN_Voltage(),2) + ","
    "\"IN_I\":" + String(CSPS_IN_Current(),2) + ","
    "\"IN_P\":" + String(CSPS_IN_Power(),2) + ","
    "\"OUT_V\":" + String(CSPS_OUT_Voltage(),2) + ","
    "\"OUT_I\":" + String(CSPS_OUT_Current(),2) + ","
    "\"OUT_P\":" + String(CSPS_OUT_Power(),2) + ","
    "\"T0\":" + String(CSPS_Temp0(),2) + ","
    "\"T1\":" + String(CSPS_Temp1(),2) + ","
    "\"Fan\":" + String(CSPS_Fan()) + ","
    "\"Time\":" + String(CSPS_Time(),2) +
  "}";
}

// 工具
int analysis_Serial_to_int(String in, String key) {
  int p = in.indexOf(key) + key.length();
  return in.substring(p).toInt();
}

void test_csps() {
  Serial.println(GET_INFO());
  Serial.println(GET_Power_INFO());
}
