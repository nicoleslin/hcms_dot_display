#include <Arduino.h>
#include <EEPROM.h>
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

#define EEPROM_START_ADDR 0x00 // offset from 0x1400 
#define NUM_COL 5
#define NUM_ROW 7
#define NUM_CHAR 16
#define MAX_PERIOD_US 1000 // actual max is 2000

uint8_t n_pot_val = 0;
unsigned long pot_start_time = 0;

unsigned long col_start_time = 0;
short col_pin_array[5] = {MCU_COL_1_PIN, MCU_COL_2_PIN, MCU_COL_3_PIN, MCU_COL_4_PIN, MCU_COL_5_PIN};
uint16_t col_on_us = 10000; // us
uint16_t num_row_bits = NUM_ROW * NUM_CHAR;

short EEPROM_WRITE_FLAG = 1;
unsigned long shift_us = 0;
short data_bit = 0;

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

  analogReadResolution(8);
  // since the COL_PWM FET is PMOS, the duty cycle is (1 - (3V3/5V))*255 = 86
  analogWrite(COL_PWM_PIN, 86);

  digitalWrite(CLK_PIN, HIGH);
  digitalWrite(nBLNK_PIN, HIGH);
  digitalWrite(MCU_COL_1_PIN, HIGH);
  digitalWrite(MCU_COL_2_PIN, HIGH);
  digitalWrite(MCU_COL_3_PIN, HIGH);
  digitalWrite(MCU_COL_4_PIN, HIGH);
  digitalWrite(MCU_COL_5_PIN, HIGH);
}

void loop() {
  // need to meet timing to pulse each column at a minimum of 100Hz, refresh rate of 150Hz is 4ms
  // The row data for a given column (one 7 bit byte per character) is loaded (bit serial) into the on-board 28 (56) bit shift register with high to low transitions of the Clock input.
  // load all row data for col 1 (7 bits per character, 16 characters, 112 bits)
  // illuminate col 1
  // repeat for col 2-5

  for(int i = 0; i < NUM_COL; i++)
  {
    data_bit = 0;
    col_start_time = micros();
    for(int j = 0; j < num_row_bits; j++) 
    {
      digitalWrite(MCU_DATA_PIN, data_bit);
      delayMicroseconds(100);
      digitalWrite(CLK_PIN, LOW);
      data_bit = !data_bit; // toggle the data bit to achieve checkerboard
      delayMicroseconds(200);
      digitalWrite(CLK_PIN, HIGH);
    }
    shift_us = micros() - col_start_time;
    // digitalWrite(nBLNK_PIN, HIGH);
    digitalWrite(col_pin_array[i], LOW);
    if (EEPROM_WRITE_FLAG == 1){
      EEPROM.write(EEPROM_START_ADDR, shift_us);
      EEPROM_WRITE_FLAG = 0;
    }
    delayMicroseconds(col_on_us);
    digitalWrite(col_pin_array[i], HIGH);
    // digitalWrite(nBLNK_PIN, LOW);
    // delayMicroseconds(MAX_PERIOD_US - col_on_us);
  }
  
  
}
