/*
    MIT License

    Copyright (c) 2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "lcd_il9163.h"
#include "lcd_common.h"
#include "intf/ssd1306_interface.h"
#include "spi/ssd1306_spi.h"
#include "ssd1306_hal/io.h"
#include "nano_gfx_types.h"

#define CMD_ARG     0xFF

extern uint16_t ssd1306_color;
extern uint32_t s_ssd1306_spi_clock;

static uint8_t s_rotation = 0x00;
static uint8_t s_rgb_bit  = 0b00001000;

static const PROGMEM uint8_t s_oled128x128_initData[] =
{
//    0x01,                     // sw reset. not needed, we do hardware reset
    0x11,                       // exit sleep mode
    0x3A, CMD_ARG, 0x05,        // set 16-bit pixel format
    0x26, CMD_ARG, 0x04,        // set gamma curve: valid values 1, 2, 4, 8
//    0xF2, CMD_ARG, 0x01,        // enable gamma adjustment, 0 - to disable
//    0xE0, CMD_ARG, 0x3F, CMD_ARG, 0x25, CMD_ARG, 0x1C,
//          CMD_ARG, 0x1E, CMD_ARG, 0x20, CMD_ARG, 0x12,
//          CMD_ARG, 0x2A, CMD_ARG, 0x90, CMD_ARG, 0x24,
//          CMD_ARG, 0x11, CMD_ARG, 0x00, CMD_ARG, 0x00,
//          CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, // positive gamma correction
//    0xE1, CMD_ARG, 0x20, CMD_ARG, 0x20, CMD_ARG, 0x20,
//          CMD_ARG, 0x20, CMD_ARG, 0x05, CMD_ARG, 0x00,
//          CMD_ARG, 0x15, CMD_ARG, 0xA7, CMD_ARG, 0x3D,
//          CMD_ARG, 0x18, CMD_ARG, 0x25, CMD_ARG, 0x2A,
//          CMD_ARG, 0x2B, CMD_ARG, 0x2B, CMD_ARG, 0x3A, // negative gamma correction
//    0xB1,  CMD_ARG,  0x08, CMD_ARG, 0x08, // frame rate control 1, use by default
//    0xB4,  CMD_ARG, 0x07,                 // display inversion, use by default
    0xC0,  CMD_ARG,  0x0A, CMD_ARG, 0x02, // power control 1
    0xC1,  CMD_ARG,  0x02,                // power control 2
    0xC5,  CMD_ARG,  0x50, CMD_ARG, 0x5B, // vcom control 1
    0xC7,  CMD_ARG,  0x40,                // vcom offset
//    0x2A,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x7F,   // set column address, not needed. set by direct API
//    0x2B,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x9F,   // set page address, not needed. set by direct API
    0x36,  CMD_ARG,  0b00101000,          // enable fake "vertical addressing" mode (for il9163_setBlock() )
    0x29,                                 // display on
};

static const PROGMEM uint8_t s_oled128x160_initData[] =
{
//    0x01,                     // sw reset. not needed, we do hardware reset
    0x11,                       // exit sleep mode
//    0x28,                                 // display off
    0x3A, CMD_ARG, 0x05,        // set 16-bit pixel format
    0x26, CMD_ARG, 0x04,        // set gamma curve: valid values 1, 2, 4, 8
    0xF2, CMD_ARG, 0x01,        // enable gamma adjustment, 0 - to disable
    0xE0, CMD_ARG, 0x3F, CMD_ARG, 0x25, CMD_ARG, 0x1C,
          CMD_ARG, 0x1E, CMD_ARG, 0x20, CMD_ARG, 0x12,
          CMD_ARG, 0x2A, CMD_ARG, 0x90, CMD_ARG, 0x24,
          CMD_ARG, 0x11, CMD_ARG, 0x00, CMD_ARG, 0x00,
          CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, // positive gamma correction
    0xE1, CMD_ARG, 0x20, CMD_ARG, 0x20, CMD_ARG, 0x20,
          CMD_ARG, 0x20, CMD_ARG, 0x05, CMD_ARG, 0x00,
          CMD_ARG, 0x15, CMD_ARG, 0xA7, CMD_ARG, 0x3D,
          CMD_ARG, 0x18, CMD_ARG, 0x25, CMD_ARG, 0x2A,
          CMD_ARG, 0x2B, CMD_ARG, 0x2B, CMD_ARG, 0x3A, // negative gamma correction
    0xB1,  CMD_ARG,  0x08, CMD_ARG, 0x08, // frame rate control 1, use by default
    0xB4,  CMD_ARG, 0x07,                 // display inversion, use by default
    0xC0,  CMD_ARG,  0x0A, CMD_ARG, 0x02, // power control 1
    0xC1,  CMD_ARG,  0x02,                // power control 2
    0xC5,  CMD_ARG,  0x50, CMD_ARG, 0x5B, // vcom control 1
    0xC7,  CMD_ARG,  0x40,                // vcom offset
//    0x2A,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x7F,   // set column address, not needed. set by direct API
//    0x2B,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x9F,   // set page address, not needed. set by direct API
    0x36,  CMD_ARG,  0b00100000,          // enable fake "vertical addressing" mode (for il9163_setBlock() )
    0x29,                                 // display on
};

static uint8_t s_column;
static uint8_t s_page;

static void il9163_setBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    s_column = x;
    s_page = y;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2B);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(x + (s_rotation == 3 ? 32 : 0));
    ssd1306_intf.send(0);
    ssd1306_intf.send((rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1))
                       + (s_rotation == 3 ? 32 : 0));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2A);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send((y<<3) + (s_rotation == 2 ? 32: 0));
    ssd1306_intf.send(0);
    ssd1306_intf.send((((y<<3) + 7) < ssd1306_lcd.height ? ((y<<3) + 7) : (ssd1306_lcd.height - 1))
                      + (s_rotation == 2 ? 32: 0));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2C);
    ssd1306_spiDataMode(1);
}

static void il9163_setBlock2(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2A);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(x + (s_rotation == 7 ? 32 : 0));
    ssd1306_intf.send(0);
    ssd1306_intf.send((rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1))
                       + (s_rotation == 7 ? 32 : 0));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2B);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(y + (s_rotation == 6 ? 32: 0));
    ssd1306_intf.send(0);
    ssd1306_intf.send(ssd1306_lcd.height - 1 + (s_rotation == 6 ? 32: 0));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2C);
    ssd1306_spiDataMode(1);
}

static void il9163_nextPage(void)
{
    ssd1306_intf.stop();
    ssd1306_lcd.set_block(s_column,s_page+1,0);
}

static void il9163_nextPage2(void)
{
}

void    il9163_setMode(lcd_mode_t mode)
{
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send( 0x36 );
    ssd1306_spiDataMode(1);
    ssd1306_intf.send( mode ? 0b00101000 : 0b00001000 );
    ssd1306_intf.stop();
    if (mode)
    {
        ssd1306_lcd.set_block = il9163_setBlock;
        ssd1306_lcd.next_page = il9163_nextPage;
    }
    else
    {
        ssd1306_lcd.set_block = il9163_setBlock2;
        ssd1306_lcd.next_page = il9163_nextPage2;
    }
    s_rotation = mode ? 0x00 : 0x04;
}

static void il9163_sendPixels(uint8_t data)
{
    for (uint8_t i=8; i>0; i--)
    {
        if ( data & 0x01 )
        {
            ssd1306_intf.send( (uint8_t)(ssd1306_color>>8) );
            ssd1306_intf.send( (uint8_t)(ssd1306_color) );
        }
        else
        {
            ssd1306_intf.send( 0B00000000 );
            ssd1306_intf.send( 0B00000000 );
        }
        data >>= 1;
    }
}

static void il9163_sendPixelsBuffer(const uint8_t *buffer, uint16_t len)
{
    while(len--)
    {
        il9163_sendPixels(*buffer);
        buffer++;
    }
}

static void il9163_sendPixel8(uint8_t data)
{
    uint16_t color = RGB8_TO_RGB16(data);
    ssd1306_intf.send( color >> 8 );
    ssd1306_intf.send( color & 0xFF );
}

void    il9163_128x128_init()
{
    ssd1306_lcd.type = LCD_TYPE_SSD1331;
    ssd1306_lcd.height = 128;
    ssd1306_lcd.width = 128;
    s_rgb_bit = 0b00001000; // set BGR mode mapping
    ssd1306_lcd.set_block = il9163_setBlock;
    ssd1306_lcd.next_page = il9163_nextPage;
    ssd1306_lcd.send_pixels1  = il9163_sendPixels;
    ssd1306_lcd.send_pixels_buffer1 = il9163_sendPixelsBuffer;
    ssd1306_lcd.send_pixels8 = il9163_sendPixel8;
    ssd1306_lcd.set_mode = il9163_setMode;
    ssd1306_configureSpiDisplay(s_oled128x128_initData, sizeof(s_oled128x128_initData));
}

void   il9163_128x128_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        pinMode(rstPin, OUTPUT);
        digitalWrite(rstPin, HIGH);
        /* Wait at least 1ms after VCC is up for LCD */
        delay(1);
        /* Perform reset operation of LCD display */
        digitalWrite(rstPin, LOW);
        delay(20);
        digitalWrite(rstPin, HIGH);
    }
    /* ssd1351 cannot work faster than at 4MHz per datasheet */
    s_ssd1306_spi_clock = 8000000;
    ssd1306_spiInit(cesPin, dcPin);
    il9163_128x128_init();
}

void il9163_setRotation(uint8_t rotation)
{
    uint8_t ram_mode;
    if ((rotation^s_rotation) & 0x01)
    {
        uint16_t t = ssd1306_lcd.width;
        ssd1306_lcd.width = ssd1306_lcd.height;
        ssd1306_lcd.height = t;
    }
    s_rotation = (rotation & 0x03) | (s_rotation & 0x04);
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x28);
    ssd1306_intf.send(0x36);
    ssd1306_spiDataMode(1);
    switch (s_rotation)
    {
    case 0:
        ram_mode = 0b00100000;
        break;
    case 1: // 90 degree CW
        ram_mode = 0b01000000;
        break;
    case 2: // 180 degree CW
        ram_mode = 0b11100000;
        break;
    case 3: // 270 degree CW
        ram_mode = 0b10000000;
        break;
    case 4:
        ram_mode = 0b00000000;
        break;
    case 5: // 90 degree CW
        ram_mode = 0b01100000;
        break;
    case 6: // 180 degree CW
        ram_mode = 0b11000000;
        break;
    default: // 270 degree CW
        ram_mode = 0b10100000;
        break;
    }
    ssd1306_intf.send( ram_mode | s_rgb_bit );
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x29);
    ssd1306_intf.stop();
}

////////////////////////////////////////////////////////////////////////////////////////
//                         ST7735 support
////////////////////////////////////////////////////////////////////////////////////////

static void st7735_setBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    s_column = x;
    s_page = y;
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2B);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(x);
    ssd1306_intf.send(0);
    ssd1306_intf.send((rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1)));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2A);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send((y<<3));
    ssd1306_intf.send(0);
    ssd1306_intf.send((((y<<3) + 7) < ssd1306_lcd.height ? ((y<<3) + 7) : (ssd1306_lcd.height - 1)));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2C);
    ssd1306_spiDataMode(1);
}

#if 0
static void st7735_setBlock2(lcduint_t x, lcduint_t y, lcduint_t w)
{
    uint8_t rx = w ? (x + w - 1) : (ssd1306_lcd.width - 1);
    ssd1306_intf.start();
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2A);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(x);
    ssd1306_intf.send(0);
    ssd1306_intf.send((rx < ssd1306_lcd.width ? rx : (ssd1306_lcd.width - 1)));
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2B);
    ssd1306_spiDataMode(1);  // According to datasheet all args must be passed in data mode
    ssd1306_intf.send(0);
    ssd1306_intf.send(y);
    ssd1306_intf.send(0);
    ssd1306_intf.send(ssd1306_lcd.height - 1);
    ssd1306_spiDataMode(0);
    ssd1306_intf.send(0x2C);
    ssd1306_spiDataMode(1);
}
#endif

void    st7735_128x160_init()
{
    ssd1306_lcd.type = LCD_TYPE_SSD1331;
    ssd1306_lcd.width = 128;
    ssd1306_lcd.height = 160;
    s_rgb_bit = 0b00000000; // set RGB mode mapping
    ssd1306_lcd.set_block = st7735_setBlock;
    ssd1306_lcd.next_page = il9163_nextPage;
    ssd1306_lcd.send_pixels1  = il9163_sendPixels;
    ssd1306_lcd.send_pixels_buffer1 = il9163_sendPixelsBuffer;
    ssd1306_lcd.send_pixels8 = il9163_sendPixel8;
    ssd1306_lcd.set_mode = il9163_setMode;
    ssd1306_configureSpiDisplay(s_oled128x160_initData, sizeof(s_oled128x160_initData));
}

void   st7735_128x160_spi_init(int8_t rstPin, int8_t cesPin, int8_t dcPin)
{
    if (rstPin >=0)
    {
        pinMode(rstPin, OUTPUT);
        digitalWrite(rstPin, HIGH);
        /* Wait at least 1ms after VCC is up for LCD */
        delay(1);
        /* Perform reset operation of LCD display */
        digitalWrite(rstPin, LOW);
        delay(20);
        digitalWrite(rstPin, HIGH);
        /* Give 120ms display to initialize */
        delay(120);
    }
    /* ssd1351 cannot work faster than at 4MHz per datasheet */
    s_ssd1306_spi_clock = 8000000;
    ssd1306_spiInit(cesPin, dcPin);
    st7735_128x160_init();
}
