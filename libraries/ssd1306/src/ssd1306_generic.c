/*
    MIT License

    Copyright (c) 2016-2018, Alexey Dynda

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

///////////////////////////////////////////////////////////////////////////////
////// GENERIC FUNCTIONS APPLICABLE FOR ALL DISPLAY TYPES /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "font6x8.h"
#include "ssd1306.h"
#include "lcd/lcd_common.h"
#include "i2c/ssd1306_i2c.h"
#include "spi/ssd1306_spi.h"
#include "intf/ssd1306_interface.h"
#include "ssd1306_hal/io.h"

// TODO: remove
#include "lcd/ssd1306_commands.h"

#define swap_data(a, b)  { uint8_t t = a; a = b; b = t; }

uint16_t ssd1306_color = 0xFFFF;
static uint8_t s_invertByte = 0x00000000;
const uint8_t *s_font6x8 = &ssd1306xled_font6x8[4];
SFixedFontInfo s_fixedFont = { 0 };

uint8_t      ssd1306_displayHeight()
{
    return ssd1306_lcd.height;
}

uint8_t      ssd1306_displayWidth()
{
    return ssd1306_lcd.width;
}

void ssd1306_fillScreen(uint8_t fill_Data)
{
    fill_Data ^= s_invertByte;
    ssd1306_lcd.set_block(0, 0, 0);
    for(uint8_t m=(ssd1306_lcd.height >> 3); m>0; m--)
    {
        for(uint8_t n=ssd1306_lcd.width; n>0; n--)
        {
            ssd1306_lcd.send_pixels1(fill_Data);
        }
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}

void ssd1306_clearScreen()
{
    ssd1306_lcd.set_block(0, 0, 0);
    for(uint8_t m=(ssd1306_lcd.height >> 3); m>0; m--)
    {
        for(uint8_t n=ssd1306_lcd.width; n>0; n--)
        {
            ssd1306_lcd.send_pixels1( s_invertByte );
        }
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}


void ssd1306_displayOff()
{
    ssd1306_sendCommand(SSD1306_DISPLAYOFF);
}


void ssd1306_displayOn()
{
    ssd1306_sendCommand(SSD1306_DISPLAYON);
}

void ssd1306_setContrast(uint8_t contrast)
{
    ssd1306_commandStart();
    ssd1306_intf.send(SSD1306_SETCONTRAST);
    ssd1306_intf.send(contrast);
    ssd1306_intf.stop();
}

uint8_t ssd1306_printFixed(uint8_t xpos, uint8_t y, const char *ch, EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
    for(;;)
    {
        uint8_t c;
        uint8_t ldata;
        uint16_t offset;
        if( (x > ssd1306_lcd.width - s_fixedFont.width) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (ssd1306_lcd.height >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == s_fixedFont.pages)
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            ssd1306_intf.stop();
            ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
        }
        c = ch[j];
        if ( c >= s_fixedFont.ascii_offset )
        {
            c -= s_fixedFont.ascii_offset;
        }
        ldata = 0;
        offset = (c * s_fixedFont.pages + page_offset) * s_fixedFont.width;
        for( i=s_fixedFont.width; i>0; i--)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_fixedFont.data[offset]);
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.data[offset]);
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.data[offset + 1]);
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            ssd1306_lcd.send_pixels1(data^s_invertByte);
            offset++;
        }
        x += s_fixedFont.width;
        j++;
    }
    ssd1306_intf.stop();
    return j;
}

uint8_t ssd1306_printFixed2x(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
    for(;;)
    {
        uint8_t c;
        uint8_t ldata;
        uint16_t offset;
        if( (x > ssd1306_lcd.width - (s_fixedFont.width << 1)) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (ssd1306_lcd.height >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == (s_fixedFont.pages << 1))
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            ssd1306_intf.stop();
            ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
        }
        c = ch[j];
        if ( c >= s_fixedFont.ascii_offset )
        {
            c -= s_fixedFont.ascii_offset;
        }
        ldata = 0;
        offset = (c * s_fixedFont.pages + (page_offset >> 1)) * s_fixedFont.width;
        for( i=s_fixedFont.width; i>0; i--)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_fixedFont.data[offset]);
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.data[offset]);
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.data[offset + 1]);
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            if (page_offset & 1) data >>= 4;
            data = ((data & 0x01) ? 0x03: 0x00) |
                   ((data & 0x02) ? 0x0C: 0x00) |
                   ((data & 0x04) ? 0x30: 0x00) |
                   ((data & 0x08) ? 0xC0: 0x00);
            ssd1306_lcd.send_pixels1(data^s_invertByte);
            ssd1306_lcd.send_pixels1(data^s_invertByte);
            offset++;
        }
        x += (s_fixedFont.width << 1);
        j++;
    }
    ssd1306_intf.stop();
    return j;
}


uint8_t ssd1306_printFixedN(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style, uint8_t factor)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
    for(;;)
    {
        uint8_t c;
        uint8_t ldata;
        uint16_t offset;
        if( (x > ssd1306_lcd.width - (s_fixedFont.width << factor)) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (ssd1306_lcd.height >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == (s_fixedFont.pages << factor))
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            ssd1306_intf.stop();
            ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
        }
        c = ch[j];
        if ( c >= s_fixedFont.ascii_offset )
        {
            c -= s_fixedFont.ascii_offset;
        }
        ldata = 0;
        offset = (c * s_fixedFont.pages + (page_offset >> factor)) * s_fixedFont.width;
        for( i=s_fixedFont.width; i>0; i--)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_fixedFont.data[offset]);
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.data[offset]);
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = pgm_read_byte(&s_fixedFont.data[offset + 1]);
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            if ( factor > 0 )
            {
                uint8_t accum = 0;
                uint8_t mask = ~((0xFF) << (1<<factor));
                // N=0  ->   right shift is always 0
                // N=1  ->   right shift goes through 0, 4
                // N=2  ->   right shift goes through 0, 2, 4, 6
                // N=3  ->   right shift goes through 0, 1, 2, 3, 4, 5, 6, 7
                data >>= ((page_offset & ((1<<factor) - 1))<<(3-factor));
                for (uint8_t idx = 0; idx < 1<<(3-factor); idx++)
                {
                     accum |= (((data>>idx) & 0x01) ? (mask<<(idx<<factor)) : 0);
                }
                data = accum;
            }
            for (uint8_t z=(1<<factor); z>0; z--)
            {
                ssd1306_lcd.send_pixels1(data^s_invertByte);
            }
            offset++;
        }
        x += (s_fixedFont.width << factor);
        j++;
    }
    ssd1306_intf.stop();
    return j;
}

size_t ssd1306_write(uint8_t ch)
{
    static lcduint_t xpos = 0;
    static lcduint_t ypos = 0;
    if (ch == '\r')
    {
        xpos = 0;
        return 0;
    }
    else if ( (xpos > ssd1306_lcd.width - s_fixedFont.width) || (ch == '\n') )
    {
        xpos = 0;
        ypos += s_fixedFont.height;
        if ( ypos > ssd1306_lcd.height - s_fixedFont.height )
        {
            ypos = 0;
        }
        ssd1306_clearBlock(0, ypos>>3, ssd1306_lcd.width, s_fixedFont.height);
        if (ch == '\n')
        {
            return 0;
        }
    }
    ch -= s_fixedFont.ascii_offset;
    ssd1306_drawBitmap( xpos,
                        ypos>>3,
                        s_fixedFont.width,
                        s_fixedFont.height,
                       &s_fixedFont.data[ ch * s_fixedFont.pages * s_fixedFont.width ] );
    xpos += s_fixedFont.width;
    return 1;
}

size_t ssd1306_print(const char ch[])
{
    size_t n = 0;
    while (*ch)
    {
        n += ssd1306_write(*ch);
        ch++;
    }
    return n;
}

uint8_t ssd1306_charF6x8(uint8_t x, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j=0;
    ssd1306_lcd.set_block(x, y, ssd1306_lcd.width - x);
    while(ch[j] != '\0')
    {
        uint8_t ldata;
        uint8_t c = ch[j] - 32;
        if ( c > 224 )
        {
            c = 0;
        }
        if(x > ssd1306_lcd.width - 6)
        {
            x=0;
            y++;
        }
        ldata = 0;
        for(i=0;i<6;i++)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_font6x8[c*6+i]);
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = pgm_read_byte(&s_font6x8[c*6+i]);
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = pgm_read_byte(&s_font6x8[c*6+i + 1]);
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            ssd1306_lcd.send_pixels1(data^s_invertByte);
        }
        x += 6;
        j++;
    }
    ssd1306_intf.stop();
    return j;
}

uint8_t ssd1306_charF12x16(uint8_t xpos, uint8_t y, const char ch[], EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t odd = 0;
    uint8_t x = xpos;
    ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
    for(;;)
    {
        uint8_t c;
        uint8_t ldata;
        if( (x > ssd1306_lcd.width-12) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (ssd1306_lcd.height >> 3))
            {
                break;
            }
            if (odd)
            {
                text_index = j;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            odd = !odd;
            ssd1306_intf.stop();
            ssd1306_lcd.set_block(xpos, y, ssd1306_lcd.width - xpos);
        }
        c = ch[j] - 32;
        if ( c > 224 )
        {
            c = 0;
        }
        ldata = 0;
        for(i=0;i<6;i++)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&s_font6x8[c*6+i]);
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = pgm_read_byte(&s_font6x8[c*6+i]);
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = pgm_read_byte(&s_font6x8[c*6+i + 1]);
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            if (odd) data >>= 4;
            data = ((data & 0x01) ? 0x03: 0x00) |
                   ((data & 0x02) ? 0x0C: 0x00) |
                   ((data & 0x04) ? 0x30: 0x00) |
                   ((data & 0x08) ? 0xC0: 0x00);
            ssd1306_lcd.send_pixels1(data^s_invertByte);
            ssd1306_lcd.send_pixels1(data^s_invertByte);
        }
        x += 12;
        j++;
    }
    ssd1306_intf.stop();
    return j;
}

uint8_t      ssd1306_charF6x8_eol(uint8_t left,
                                  uint8_t y,
                                  const char ch[],
                                  EFontStyle style,
                                  uint8_t right)
{
    uint8_t len = ssd1306_charF6x8(left, y, ch, style);
    uint8_t text_end_pos = len * 6 + left;
    if (text_end_pos <= right)
    {
        ssd1306_clearBlock(text_end_pos, y, right - text_end_pos + 1, 8);
    }
    return len;
}

void         ssd1306_setFixedFont(const uint8_t * progmemFont)
{
    s_fixedFont.width  = pgm_read_byte(&progmemFont[1]);
    s_fixedFont.height = pgm_read_byte(&progmemFont[2]);
    s_fixedFont.pages = (s_fixedFont.height + 7) >> 3;
    s_fixedFont.ascii_offset = pgm_read_byte(&progmemFont[3]);
    s_fixedFont.data = progmemFont + 4;
}

void         ssd1306_setFont6x8(const uint8_t * progmemFont)
{
    s_font6x8 = progmemFont + 4;
}

void         ssd1306_putPixel(uint8_t x, uint8_t y)
{
    ssd1306_lcd.set_block(x, y >> 3, 1);
    ssd1306_lcd.send_pixels1((1 << (y & 0x07))^s_invertByte);
    ssd1306_intf.stop();
}

void         ssd1306_putPixels(uint8_t x, uint8_t y, uint8_t pixels)
{
    ssd1306_lcd.set_block(x, y >> 3, 1);
    ssd1306_lcd.send_pixels1(pixels^s_invertByte);
    ssd1306_intf.stop();
}

void         ssd1306_putPixel_delayed(uint8_t x, uint8_t y, uint8_t complete)
{
    static uint8_t lx = 0, ly = 0xFF;
    static uint8_t pixels = 0;
    if ((lx != x) || ((ly & 0xF8) != (y & 0xF8)) || (complete))
    {
        if (ly != 0xFF)
        {
            ssd1306_putPixels( lx, ly, pixels );
        }
        pixels = 0;
        ly = 0xFF;
    }
    if ( !complete )
    {
        pixels |= (1 << (y & 0x07));
        lx = x; ly = y;
    }
}

void         ssd1306_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    lcduint_t  dx = x1 > x2 ? (x1 - x2): (x2 - x1);
    lcduint_t  dy = y1 > y2 ? (y1 - y2): (y2 - y1);
    lcduint_t  err = 0;
    if (dy > dx)
    {
        if (y1 > y2)
        {
            swap_data(x1, x2);
            swap_data(y1, y2);
        }
        for(; y1<=y2; y1++)
        {
            err += dx;
            if (err >= dy)
            {
                 err -= dy;
                 x1 < x2 ? x1++: x1--;
            }
            ssd1306_putPixel_delayed( x1, y1, 0 );
        }
        ssd1306_putPixel_delayed( 0, 0, 1 );
    }
    else
    {
        if (x1 > x2)
        {
            swap_data(x1, x2);
            swap_data(y1, y2);
        }
        for(; x1<=x2; x1++)
        {
            err += dy;
            if (err >= dx)
            {
                 err -= dx;
                 if (y1 < y2) y1++; else y1--;
            }
            ssd1306_putPixel( x1, y1 );
        }
    }
}

void         ssd1306_drawHLine(uint8_t x1, uint8_t y1, uint8_t x2)
{
    ssd1306_lcd.set_block(x1, y1 >> 3, x2 - x1 + 1);
    for (uint8_t x = x1; x <= x2; x++)
    {
        ssd1306_lcd.send_pixels1((1 << (y1 & 0x07))^s_invertByte);
    }
    ssd1306_intf.stop();
}

void         ssd1306_drawVLine(uint8_t x1, uint8_t y1, uint8_t y2)
{
    uint8_t topPage = y1 >> 3;
    uint8_t bottomPage = y2 >> 3;
    uint8_t height = y2-y1;
    uint8_t y;
    ssd1306_lcd.set_block(x1, topPage, 1);
    if (topPage == bottomPage)
    {
        ssd1306_lcd.send_pixels1( ((0xFF >> (0x07 - height)) << (y1 & 0x07))^s_invertByte );
        ssd1306_intf.stop();
        return;
    }
    ssd1306_lcd.send_pixels1( (0xFF << (y1 & 0x07))^s_invertByte );
    for ( y = (topPage + 1); y <= (bottomPage - 1); y++)
    {
        ssd1306_lcd.next_page();
        ssd1306_lcd.send_pixels1( 0xFF^s_invertByte );
    }
    ssd1306_lcd.next_page();
    ssd1306_lcd.send_pixels1( (0xFF >> (0x07 - (y2 & 0x07)))^s_invertByte );
    ssd1306_intf.stop();
}

void         ssd1306_drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    ssd1306_drawHLine(x1+1, y1, x2-1);
    ssd1306_drawHLine(x1+1, y2, x2-1);
    ssd1306_drawVLine(x1, y1, y2);
    ssd1306_drawVLine(x2, y1, y2);
}

void ssd1306_drawBufferFast(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    uint8_t j;
    ssd1306_lcd.set_block(x, y >> 3, w);
    for(j=(h >> 3); j>0; j--)
    {
        ssd1306_lcd.send_pixels_buffer1(buf,w);
        buf+=w;
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}

void ssd1306_drawBuffer(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf)
{
    uint8_t i, j;
    ssd1306_lcd.set_block(x, y, w);
    for(j=(h >> 3); j>0; j--)
    {
        for(i=w;i>0;i--)
        {
            ssd1306_lcd.send_pixels1(s_invertByte^*buf++);
        }
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}

void ssd1306_drawBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *buf)
{
    uint8_t i, j;
    uint8_t remainder = (ssd1306_lcd.width - x) < w ? (w + x - ssd1306_lcd.width): 0;
    w -= remainder;
    ssd1306_lcd.set_block(x, y, w);
    for(j=(h >> 3); j>0; j--)
    {
        for(i=w;i>0;i--)
        {
            ssd1306_lcd.send_pixels1(s_invertByte^pgm_read_byte(buf++));
        }
        buf += remainder;
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}

void gfx_drawMonoBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    lcduint_t origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    lcduint_t i, j;
    if (y + (lcdint_t)h <= 0) return;
    if (y >= ssd1306_lcd.height) return;
    if (x + (lcdint_t)w <= 0) return;
    if (x >= ssd1306_lcd.width)  return;
    if (y < 0)
    {
         buf += ((lcduint_t)((-y) + 7) >> 3) * w;
         h += y;
         y = 0;
         complexFlag = 1;
    }
    if (x < 0)
    {
         buf += -x;
         w += x;
         x = 0;
    }
    max_pages = (lcduint_t)(h + 15 - offset) >> 3;
    if ((lcduint_t)((lcduint_t)y + h) > (lcduint_t)ssd1306_lcd.height)
    {
         h = (lcduint_t)(ssd1306_lcd.height - (lcduint_t)y);
    }
    if ((lcduint_t)((lcduint_t)x + w) > (lcduint_t)ssd1306_lcd.width)
    {
         w = (lcduint_t)(ssd1306_lcd.width - (lcduint_t)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    ssd1306_lcd.set_block(x, y >> 3, w);
    for(j=0; j < pages; j++)
    {
        if ( j == max_pages - 1 ) mainFlag = !offset;
        for( i=w; i > 0; i--)
        {
            uint8_t data = 0;
            if ( mainFlag )    data |= (pgm_read_byte(buf) << offset);
            if ( complexFlag ) data |= (pgm_read_byte(buf - origin_width) >> (8 - offset));
            buf++;
            ssd1306_lcd.send_pixels1(s_invertByte^data);
        }
        buf += origin_width - w;
        complexFlag = offset;
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}


void ssd1306_clearBlock(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    uint8_t i, j;
    ssd1306_lcd.set_block(x, y, w);
    for(j=(h >> 3); j>0; j--)
    {
        for(i=w;i>0;i--)
        {
            ssd1306_lcd.send_pixels1(s_invertByte);
        }
        ssd1306_lcd.next_page();
    }
    ssd1306_intf.stop();
}


void ssd1306_drawSpriteEx(uint8_t x, uint8_t y, uint8_t w, const uint8_t *sprite)
{
   uint8_t i;
   ssd1306_lcd.set_block(x,y,w);
   for(i=0;i<w;i++)
   {
       ssd1306_lcd.send_pixels1(s_invertByte^pgm_read_byte(&sprite[i]));
   }
   ssd1306_intf.stop();
}


void ssd1306_drawSprite(SPRITE *sprite)
{
    uint8_t offsety = sprite->y & 0x7;
    if (sprite->y < ssd1306_lcd.height)
    {
        ssd1306_lcd.set_block(sprite->x, sprite->y >> 3, sprite->w);
        for (uint8_t i=0; i < sprite->w; i++)
        {
            ssd1306_lcd.send_pixels1( s_invertByte^(pgm_read_byte( &sprite->data[i] ) << offsety) );
        }
        ssd1306_intf.stop();
    }
    if (offsety && (sprite->y + 8 < ssd1306_lcd.height))
    {
        ssd1306_lcd.set_block(sprite->x, (sprite->y >> 3) + 1, sprite->w);
        for (uint8_t i=0; i < sprite->w; i++)
        {
            ssd1306_lcd.send_pixels1( s_invertByte^(pgm_read_byte( &sprite->data[i] ) >> (8 - offsety)) );
        }
        ssd1306_intf.stop();
    }
    sprite->lx = sprite->x;
    sprite->ly = sprite->y;
}


void ssd1306_eraseSprite(SPRITE *sprite)
{
    uint8_t posy = sprite->y >> 3;
    uint8_t offsety = sprite->y & 0x7;
    ssd1306_lcd.set_block(sprite->x, posy, sprite->w);
    for (uint8_t i=sprite->w; i > 0; i--)
    {
       ssd1306_lcd.send_pixels1( s_invertByte );
    }
    ssd1306_intf.stop();
    if (offsety)
    {
        ssd1306_lcd.set_block(sprite->x, posy + 1, sprite->w);
        for (uint8_t i=sprite->w; i > 0; i--)
        {
           ssd1306_lcd.send_pixels1( s_invertByte );
        }
    }
    ssd1306_intf.stop();
}


void ssd1306_eraseTrace(SPRITE *sprite)
{
    uint8_t y1 = sprite->ly >> 3;
    uint8_t y2 = (sprite->ly + 7) >> 3;
    if (sprite->ly < sprite->y)
        y2 = min(y2, (uint8_t)((sprite->y >> 3) - 1));
    else if (sprite->y + 8 > sprite->ly)
        y1 = max(y1, (sprite->ly + 7) >> 3);
    for(uint8_t y = y1; y <= y2; y++)
    {
        ssd1306_lcd.set_block(sprite->lx, y, sprite->w);
        for(uint8_t x = sprite->w; x > 0; x--)
        {
            ssd1306_lcd.send_pixels1( s_invertByte );
        }
        ssd1306_intf.stop();
    }
    if (sprite->lx != sprite->x)
    {
        uint8_t x1 = sprite->lx;
        uint8_t x2 = sprite->lx + sprite->w - 1;
        if (sprite->x < sprite->lx)
            x1 = max(x1, sprite->x + sprite->w);
        else
            x2 = min((uint8_t)(sprite->x - 1), x2);
        for(uint8_t y = sprite->ly >> 3; y <= (sprite->ly + 7) >> 3; y++)
        {
            ssd1306_lcd.set_block(x1, y, x2 - x1 + 1 );
            for(uint8_t x = x2 - x1 + 1; x > 0; x--)
            {
                ssd1306_lcd.send_pixels1( s_invertByte );
            }
            ssd1306_intf.stop();
        }
    }
}

SPRITE       ssd1306_createSprite(uint8_t x, uint8_t y, uint8_t w, const uint8_t *data)
{
    return (SPRITE){x,y,w,x,y,data,NULL};
}

void         ssd1306_replaceSprite(SPRITE *sprite, const uint8_t *data)
{
    sprite->data = data;
}

void         ssd1306_invertMode()
{
    if (ssd1306_lcd.type != LCD_TYPE_SSD1331)
    { 
        ssd1306_sendCommand(SSD1306_INVERTDISPLAY);
    }
}

void         ssd1306_normalMode()
{
    if (ssd1306_lcd.type != LCD_TYPE_SSD1331)
    { 
        ssd1306_sendCommand(SSD1306_NORMALDISPLAY);
    }
}

void         ssd1306_flipHorizontal(uint8_t mode)
{
    if (ssd1306_lcd.type != LCD_TYPE_SSD1331)
    { 
         ssd1306_sendCommand( SSD1306_SEGREMAP | (mode ? 0x00: 0x01 ) );
    }
}

void         ssd1306_flipVertical(uint8_t mode)
{
    if (ssd1306_lcd.type != LCD_TYPE_SSD1331)
    { 
         ssd1306_sendCommand( mode ? SSD1306_COMSCANINC : SSD1306_COMSCANDEC );
    }
}
 
void         ssd1306_negativeMode()
{
    s_invertByte = 0xFF;
}

void         ssd1306_positiveMode()
{
    s_invertByte = 0x00;
}
