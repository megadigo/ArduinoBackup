/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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
/**
 * @file lcd_common.h global lcd settings
 */

#ifndef _LCD_COMMON_H_
#define _LCD_COMMON_H_

#include "ssd1306_hal/io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LCD_INTERFACE_API LCD High level API
 * @{
 * @brief LCD high level API for setup and programming GDRAM
 *
 * @details This group contains API functions for OLED displays initialization and
 *          direct programming of GDRAM. This API can be used to create your own
 *          graphics functions.
 */

/**
 * Enumeration, describing display type
 */
typedef enum
{
    /** Default type of LCD display: ssd1306 oled */
    LCD_TYPE_SSD1306,
    /** Experimental type of LCD display: pcd8544 led */
    LCD_TYPE_PCD8544,
    /** Support for sh1106 OLED display */
    LCD_TYPE_SH1106,
    /** Default type of LCD display: ssd1331 oled */
    LCD_TYPE_SSD1331,
    /** User LCD type */
    LCD_TYPE_CUSTOM,
} lcd_type_t;

/**
 * Available lcd modes used by the library.
 * LCD_MODE_SSD1306_COMPAT is compatible mode, which should be used
 * with standard monochrome functions.
 */
typedef enum
{
    /**
     * Normal mode RGB displays. All ssd1306 monochrome direct draw
     * functions do not work in this mode.
     */
    LCD_MODE_NORMAL = 0,

    /**
     * ssd1306 compatible mode. This is special mode, that allows
     * to use ssd1306 monochrome direct draw functions, but RGB
     * functions will not work.
     */
    LCD_MODE_SSD1306_COMPAT = 1,
} lcd_mode_t;

/**
 * Structure, describing display driver configuration
 */
typedef struct
{
    /** Current selected lcd display type */
    lcd_type_t type;

    /** Current display width */
    lcduint_t  width;

    /** Current display height */
    lcduint_t  height;

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * For ssd1306 it uses horizontal addressing mode, while for
     * sh1106 the function uses page addressing mode.
     * Width can be specified as 0, thus the library will set the right boundary to
     * region of RAM block to the right column of the display.
     * @param x - column (left region)
     * @param y - page (top page of the block)
     * @param w - width of the block in pixels to control
     *
     * @warning - this function initiates session (i2c or spi) and do not close it.
     *            To close session, please, use ssd1306_intf.stop().
     */
    void (*set_block)(lcduint_t x, lcduint_t y, lcduint_t w);

    /**
     * Switches to the start of next RAM page for the block, specified by
     * set_block().
     * For ssd1306 it does nothing, while for sh1106 the function moves cursor to
     * next page.
     */
    void (*next_page)(void);

    /**
     * Sends 8 monochrome vertical pixels to OLED driver.
     * @param data - byte, representing 8 pixels.
     */
    void (*send_pixels1)(uint8_t data);

    /**
     * Sends buffer containing 8 monochrome vertical pixels, encoded in each byte.
     * @param buffer - buffer containing monochrome pixels.
     * @param len - length of buffer in bytes.
     */
    void (*send_pixels_buffer1)(const uint8_t *buffer, uint16_t len);

    /**
     * @brief Sends RGB pixel encoded in 3-3-2 format to OLED driver.
     * Sends RGB pixel encoded in 3-3-2 format to OLED driver.
     * @param data - byte, representing RGB8 pixel.
     */
    void (*send_pixels8)(uint8_t data);

    /**
     * @brief Sets library display mode for direct draw functions.
     *
     * Sets library display mode for direct draw functions.
     * There are currently 2 modes supported: LCD_MODE_SSD1306_COMPAT and
     * LCD_MODE_NORMAL. In general, ssd1306 compatible mode uses different GDRAM
     * addressing mode, than normal mode, intended for using with RBG full-color functions.
     *
     * @param mode lcd mode to activate.
     * @see LCD_MODE_SSD1306_COMPAT
     * @see LCD_MODE_NORMAL
     * @see lcd_mode_t
     */
    void (*set_mode)(lcd_mode_t mode);
} ssd1306_lcd_t;

/**
 * Structure containing callback to low level function for currently enabled display
 */
extern ssd1306_lcd_t ssd1306_lcd;

/**
 * Current display height
 * @deprecated Use ssd1306_lcd.height instead.
 */
#define s_displayHeight   ssd1306_lcd.height

/**
 * Current display width
 * @deprecated Use ssd1306_lcd.width instead.
 */
#define s_displayWidth    ssd1306_lcd.width

/**
 * Current selected lcd display type
 * @deprecated Use ssd1306_lcd.type instead.
 */
#define g_lcd_type  ssd1306_lcd.type

/**
 * Sends byte data to SSD1306 controller memory.
 * Performs 3 operations at once: ssd1306_intf.start(), ssd1306_intf.send(), ssd1306_intf.stop();
 * @param data - byte to send to the controller memory
 * @note At present this function is used only in Arkanoid demo.
 * @deprecated There is wide variaty of functions, that can be used for this.
 */
void         ssd1306_sendData(uint8_t data) __attribute__ ((deprecated));

/**
 * @brief Sets block in RAM of lcd display controller to write data to.
 *
 * Sets block in RAM of lcd display controller to write data to.
 * For ssd1306 it uses horizontal addressing mode, while for
 * sh1106 the function uses page addressing mode.
 * Width can be specified as 0, thus the library will set the right boundary to
 * region of RAM block to the right column of the display.
 * @param x - column (left region)
 * @param y - page (top page of the block)
 * @param w - width of the block in pixels to control
 *
 * @deprecated Use ssd1306_lcd.set_block() instead.
 * @warning - this function initiates session (i2c or spi) and do not close it.
 *            To close session, please, use ssd1306_intf.stop().
 */
#define ssd1306_setRamBlock   ssd1306_lcd.set_block

/**
 * Switches to the start of next RAM page for the block, specified by
 * ssd1306_setRamBlock().
 * For ssd1306 it does nothing, while for sh1106 the function moves cursor to
 * next page.
 * @deprecated Use ssd1306_lcd.next_page() instead.
 */
#define ssd1306_nextRamPage   ssd1306_lcd.next_page

/**
 * Sends 8 monochrome vertical pixels to OLED driver.
 * @param data - byte, representing 8 pixels.
 * @deprecated Use ssd1306_lcd.send_pixels1() instead.
 */
#define ssd1306_sendPixels    ssd1306_lcd.send_pixels1

/**
 * Sends buffer containing 8 monochrome vertical pixels, encoded in each byte.
 * @param buffer - buffer containing monochrome pixels.
 * @param len - length of buffer in bytes.
 * @deprecated Use ssd1306_lcd.send_pixels_buffer1() instead.
 */
#define ssd1306_sendPixelsBuffer  ssd1306_lcd.send_pixels_buffer1

/**
 * @brief Sends RGB pixel encoded in 3-3-2 format to OLED driver.
 * Sends RGB pixel encoded in 3-3-2 format to OLED driver.
 * @param data - byte, representing RGB8 pixel.
 * @deprecated Use ssd1306_lcd.send_pixels8() instead.
 */
#define ssd1306_sendPixel8        ssd1306_lcd.send_pixels8

/**
 * @brief Sends configuration being passed to lcd display i2c/spi controller.
 *
 * Sends configuration being passed to lcd display i2c/spi controller.
 * The data bytes are sent to lcd controller as is. In case of spi display
 * this function sends cmd arguments in command mode. If lcd controller requires
 * arguments to be sent in data mode, please use ssd1306_configureSpiDisplay().
 *
 * @param config configuration, located in flash, to send to i2c/spi controller.
 * @param configSize - size of configuration data in bytes.
 */
void ssd1306_configureI2cDisplay(const uint8_t *config, uint8_t configSize);

/**
 * @brief Sends configuration being passed to lcd display spi controller.
 *
 * Sends configuration being passed to lcd display spi controller. If data byte value
 * to be sent is less than 255, then data byte is sent in command mode. If data byte
 * is 0xFF, the function does't send it to controller, but switches to spi data mode,
 * and next byte after will be sent in data spi mode. Then the function will switch back
 * to command mode. If lcd controller requires cmd arguments to be sent in command mode,
 * please use ssd1306_configureI2cDisplay().
 *
 * @param config configuration, located in flash, to send to i2c/spi controller.
 * @param configSize - size of configuration data in bytes.
 */
void ssd1306_configureSpiDisplay(const uint8_t *config, uint8_t configSize);

/**
 * @brief Sets library display mode for direct draw functions.
 *
 * Sets library display mode for direct draw functions.
 * There are currently 2 modes supported: LCD_MODE_SSD1306_COMPAT and
 * LCD_MODE_NORMAL. In general, ssd1306 compatible mode uses different GDRAM
 * addressing mode, than normal mode, intended for using with RBG full-color functions.
 *
 * @param mode lcd mode to activate.
 * @see LCD_MODE_SSD1306_COMPAT
 * @see LCD_MODE_NORMAL
 * @see lcd_mode_t
 */
void ssd1306_setMode(lcd_mode_t mode);

/**
 * @}
 */


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _LCD_COMMON_H_ */
