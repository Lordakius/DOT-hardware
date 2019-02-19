#include <Arduino.h>
#include <Display.h>

I2C_LCD LCD;
uint8_t I2C_LCD_ADDRESS = 0x51; //Device address configuration, the default value is 0x51.

Display::Display()
{
    Wire.begin();
    LCD.CleanAll(WHITE);    //Clean the screen with black or white.
    delay(1000);            //Delay for 1s.

    //8*16 font size auto new line black character on white back ground.
    LCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC); 
    LCD.WorkingModeConf(ON, ON, WM_RamMode);
}

void Display::print(char* text)
{
    LCD.CharGotoXY(0,0);    //Set the start coordinate.
    LCD.print(text);        //Display text
}

static void write_bytes_to_display_RAM(uint8_t * buf, uint8_t len, uint8_t x, uint8_t y)
{
    LCD.WorkingModeConf(ON, ON, WM_RamMode);
    LCD.WriteSeriesDispRAM(buf,len, x, y );
}

static void write_bytes_line_to_display(uint8_t * buf, uint8_t line_number) 
{
    LCD.WorkingModeConf(ON, ON, WM_RamMode);
    LCD.WriteSeriesDispRAM(buf,128, 0, line_number );
}

static void write_32_byte_with_transmutation(uint8_t * buf, uint8_t segment_number) {
    uint8_t y = (uint8_t) 7 - segment_number/4;
    uint8_t x = (segment_number%4)*32;

    write_bytes_to_display_RAM(buf, 32, x, y);
}

static uint8_t get_bit_k_at_n (uint8_t k, uint8_t n, uint8_t byte){
    uint8_t mask = 1 << k;
    return ((uint8_t)(((mask & byte) >> k) << n));
}
