#include <Arduino.h>
#define SW_CONFIG_0_PIN 14 // PA1
#define SW_CONFIG_1_PIN 2  // PA6
#define SW_CONFIG_2_PIN 0  // PA4
#define POT_PIN         15 // PA2
#define CLK_PIN         1  // PA5
#define COL_PWM_PIN     16 // PA3
#define nBLNK_PIN       3  // PA7
#define MCU_COL_1_PIN   9  // PB0
#define MCU_COL_2_PIN   8  // PB1
#define MCU_COL_3_PIN   7  // PB2
#define MCU_COL_4_PIN   6  // PB3
#define MCU_COL_5_PIN   5  // PB4
#define MCU_DATA_PIN    4  // PB5

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);
  pinMode(SW_CONFIG_0_PIN, INPUT);
  pinMode(SW_CONFIG_1_PIN, INPUT);
  pinMode(SW_CONFIG_2_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(COL_PWM_PIN, OUTPUT);
  pinMode(nBLNK_PIN, OUTPUT);
  pinMode(MCU_COL_1_PIN, OUTPUT);
  pinMode(MCU_COL_2_PIN, OUTPUT);
  pinMode(MCU_COL_3_PIN, OUTPUT);
  pinMode(MCU_COL_4_PIN, OUTPUT);
  pinMode(MCU_COL_5_PIN, OUTPUT);
  pinMode(MCU_DATA_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("hi");
  // delay(1000);
}