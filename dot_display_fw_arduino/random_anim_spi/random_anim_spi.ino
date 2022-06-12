#include <Arduino.h>
#include <SPI.h>

#define SW_CONFIG_0_PIN 14 // PA1
#define SW_CONFIG_1_PIN 2  // PA6
#define SW_CONFIG_2_PIN 0  // PA4
#define POT_PIN         15 // PA2
// #define CLK_PIN         1  // PA5

// #define COL_PWM_PIN     16 // PA3
#define COL_PWM_PIN     1  // PA5
#define nBLNK_PIN       3  // PA7
#define MCU_COL_1_PIN   9  // PB0
#define MCU_COL_2_PIN   8  // PB1
#define MCU_COL_3_PIN   7  // PB2
#define MCU_COL_4_PIN   6  // PB3
#define MCU_COL_5_PIN   5  // PB4
#define MCU_DATA_PIN    4  // PB5

#define NUM_COL 5
#define NUM_ROW 7
#define NUM_CHAR 16

#define RANDOM_OUT_OF 3000
#define PROBABILITY_ON 10 // out of RANDOM_OUT_OF

uint8_t pot_val = 0;
uint8_t blink_val = 0;
unsigned long pot_start_time = 0;

unsigned long col_start_time = 0;
short col_pin_array[5] = {MCU_COL_1_PIN, MCU_COL_2_PIN, MCU_COL_3_PIN, MCU_COL_4_PIN, MCU_COL_5_PIN};
uint16_t num_row_bits = NUM_ROW * NUM_CHAR;
short data_bit = 0;

uint8_t dot_matrix[NUM_CHAR][NUM_ROW][NUM_COL] = {0};
uint8_t spi_byte = 0;
uint8_t spi_counter = 0;
uint8_t first_spi_byte = 1;
uint8_t num_dummy_bits = 0;

typedef struct {
  uint8_t counter;
  uint8_t counter_end;
  uint8_t counter_level;
} dot_t;

void display_all_ones() {
  spi_counter = num_dummy_bits;
  first_spi_byte = 1;

  for(int i = 0; i < num_row_bits; i++) {
    if (first_spi_byte && (spi_counter < 8)) {
      spi_byte = spi_byte | (1 << spi_counter);
      spi_counter = spi_counter + 1;
    }

    else if (first_spi_byte && !(spi_counter < 8)) {
      first_spi_byte = 0;
      
      // send SPI transfer
      SPI.transfer(spi_byte);
      spi_counter = 0;
      spi_byte = 0;
    }

    // not first_spi_byte, must be doing standard spi
    else if (!first_spi_byte && (spi_counter < 8)) {
      spi_byte = spi_byte | (1 << spi_counter);
      spi_counter = spi_counter + 1;
    }

    else {
      // send SPI transfer
      SPI.transfer(spi_byte);
      spi_counter = 0;
      spi_byte = 0;
    }
  }
  
}

//void reset_display(){
//  for(int i = 0; i < num_row_bits; i++) {
//    digitalWrite(CLK_PIN, HIGH);
//    digitalWrite(MCU_DATA_PIN, LOW);
//    digitalWrite(CLK_PIN, LOW);
//  }
//}

//void write_dot(int level){
//  digitalWrite(CLK_PIN, HIGH);
//  digitalWrite(MCU_DATA_PIN, level);
//  digitalWrite(CLK_PIN, LOW);
//}

/***
void display_chars(uint8_t char_matrix[NUM_CHAR][NUM_ROW][NUM_COL]){
  spi_counter = num_dummy_bits;
  first_spi_byte = 1;
  
  for(int i = 0; i < NUM_COL; i++)
  {
    for(int j = NUM_CHAR; j > 0; j--) 
    {
      for(int k = 0; k < NUM_ROW; k++)
      {
        if (first_spi_byte && (spi_counter < 8)) {

          spi_counter = spi_counter + 1;
        }

        else if (first_spi_byte && !(spi_counter < 8)) {
          first_spi_byte = 0;
          spi_counter = 0;

          // send SPI transfer
        }

        // not first_spi_byte, must be doing standard spi
        else if () {
          
        else 
        }

        // check if dot is on
        
        // if dot is not on, use random() to determine if dot should turn on
          // dot should turn on
            // change dot_level to 

          // dot should stay off

        // if dot is on, check if counter has reached its max
          // counter not at max
            // increment dot_counter by 1, call write_dot(1)

          // counter at max
            // change dot_level to 0, change dot_counter to 0, call write_dot(0)

        
      }
    }
    
    digitalWrite(col_pin_array[i], LOW);
    delayMicroseconds(blink_val);
    digitalWrite(col_pin_array[i], HIGH);
  }
}
***/

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);
  // pinMode(SW_CONFIG_0_PIN, INPUT);
  pinMode(SW_CONFIG_1_PIN, INPUT);
  pinMode(SW_CONFIG_2_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  // pinMode(CLK_PIN, OUTPUT);
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
  // digitalWrite(COL_PWM_PIN,LOW);

  // digitalWrite(CLK_PIN, HIGH);
  digitalWrite(nBLNK_PIN, HIGH);
  digitalWrite(MCU_COL_1_PIN, HIGH);
  digitalWrite(MCU_COL_2_PIN, HIGH);
  digitalWrite(MCU_COL_3_PIN, HIGH);
  digitalWrite(MCU_COL_4_PIN, HIGH);
  digitalWrite(MCU_COL_5_PIN, HIGH);

  randomSeed(analogRead(POT_PIN));

  // initialize SPI
  SPI.swap(0);
  SPI.begin();

  // determine number of dummy bits required to stuff for SPI transfers
  num_dummy_bits = num_row_bits % 8;
 
  // reset_display();
  display_all_ones();
}

void loop() {
  // need to meet timing to pulse each column at a minimum of 100Hz, refresh rate of 150Hz is 4ms
  // The row data for a given column (one 7 bit byte per character) is loaded (bit serial) into the on-board 28 (56) bit shift register with high to low transitions of the Clock input.
  // load all row data for col 1 (7 bits per character, 16 characters, 112 bits)
  // illuminate col 1
  // repeat for col 2-5

  // display_chars(dot_matrix);
}
