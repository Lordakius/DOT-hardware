#ifndef RFIDDriver_H__
#define RFIDDriver_H__

#include <Arduino.h>
#include <Wire.h>
#include <I2C_LCD.h>

/**
 * Arduino Display Library
 *
 * uses I2C_LCD12864 Grove display v1.2
 *
 */

/**
 * display->print - prints string on display
 */

class RFIDDriver
{
    public:
        Display();
        void print(char* text);
        uint8_t I2C_LCD_ADDRESS;
        I2C_LCD LCD;
};

 void write_bytes_to_display_RAM(uint8_t * buf, uint8_t len, uint8_t x, uint8_t y);
 void write_bytes_line_to_display(uint8_t * buf, uint8_t line_number);
 static uint8_t get_bit_k_at_n (uint8_t k, uint8_t n, uint8_t byte);
 void write_32_byte_with_transmutation(uint8_t * buf, uint8_t segment_number);

#endif /* RFIDDriver.h */
