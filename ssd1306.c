// Modified version of https://github.com/monpetit/nrf52-spi-i2c-master-ssd1306/blob/master/ssd1306.c

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_twi.h"
#include "nordic_common.h"
#include "binary.h"
#include "ssd1306.h"
#include "app_error.h"
#include "app_util_platform.h"
#include <stdarg.h>

static uint8_t _i2caddr, _vccstate;
static uint32_t _dc, _rs, _cs;
static int16_t _width, _height, WIDTH, HEIGHT, cursor_x, cursor_y;
static uint8_t textsize, rotation;
static uint16_t textcolor, textbgcolor;
bool wrap,   // If set, 'wrap' text at right edge of display
     _cp437; // If set, use correct CP437 charset (default is off)


/**
 * @brief TWI master instance
 *
 * Instance of TWI master driver that would be used for communication with simulated
 * eeprom memory.
 */
static const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(1);


void twi_evt_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
#if 0
    if (p_event->type == NRF_DRV_TWI_ERROR) {
        printf("E\r\n");
    }

    if (p_event->type == NRF_DRV_TWI_TX_DONE) {
        printf("T\r\n");
    }

    if (p_event->type == NRF_DRV_TWI_RX_DONE) {
        printf("R\r\n");
    }
#endif
}


ret_code_t twi_master_init(uint32_t scl, uint32_t sda)
{
    ret_code_t ret;
    const nrf_drv_twi_config_t config = {
        .scl                = scl,
        .sda                = sda,
        .frequency          = TWI_DEFAULT_CONFIG_FREQUENCY,
        .interrupt_priority = TWI_DEFAULT_CONFIG_IRQ_PRIORITY
    };

    do {
        ret = nrf_drv_twi_init(&m_twi_master, &config, /*twi_evt_handler*/NULL, NULL);
        if (NRF_SUCCESS != ret) {
            break;
        }
        nrf_drv_twi_enable(&m_twi_master);
    }
    while (0);
    return ret;
}

static volatile bool use_i2c = false;

#define _HI(p)      nrf_gpio_pin_set(p)
#define _LO(p)      nrf_gpio_pin_clear(p)

#define _HI_RS()    _HI(_rs)
#define _LO_RS()    _LO(_rs)
#define _HI_DC()    _HI(_dc)
#define _LO_DC()    _LO(_dc)
#define _HI_CS()    _HI(_cs)
#define _LO_CS()    _LO(_cs)

#include "glcdfont.c"

// the memory buffer for the LCD

static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
#if (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH > 96*16)
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
    0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
    0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
    0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
    0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
    0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
    0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
    0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
    0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
    0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
    0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
    0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
    0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
    0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (SSD1306_LCDHEIGHT == 64)
    0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
    0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
    0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
    0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
    0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
    0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
    0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
    0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
    0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
    0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
    0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
    0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
    0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
#endif
};

#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }
#define adagfxswap(a, b) { int16_t t = a; a = b; b = t; }

// Return the size of the display (per current rotation)
int16_t ssd1306_width(void)
{
    return _width;
}

int16_t ssd1306_height(void)
{
    return _height;
}

uint8_t ssd1306_char_width(void) 
{
  return textsize * 6;
}

uint8_t ssd1306_char_height(void) 
{
  return textsize * 8;
}

void set_rotation(uint8_t x)
{
    rotation = (x & 3);
    switch (rotation) {
    case 0:
    case 2:
        _width  = WIDTH;
        _height = HEIGHT;
        break;
    case 1:
    case 3:
        _width  = HEIGHT;
        _height = WIDTH;
        break;
    }
}

void ssd1306_init_spi(uint32_t dc, uint32_t rs, uint32_t cs, uint32_t clk, uint32_t mosi)
{
#if ENABLE_SPI
    _dc = dc;
    _rs = rs;
    _cs = cs;

    nrf_gpio_cfg_output(dc);
    nrf_gpio_cfg_output(rs);
    _HI_CS();
    nrf_gpio_cfg_output(rs);
    spi_init(clk, mosi);
#endif
}

void ssd1306_init_i2c(uint32_t scl, uint32_t sda)
{
    _i2caddr = SSD1306_I2C_ADDRESS;
    use_i2c = true;
    twi_master_init(scl, sda);
}

void ssd1306_command(uint8_t c)
{
    if (use_i2c) {
        ret_code_t ret;
        uint8_t dta_send[] = {0x00, c};
        ret = nrf_drv_twi_tx(&m_twi_master, _i2caddr, dta_send, 2, false);
        UNUSED_VARIABLE(ret);
    }
    else {
#if ENABLE_SPI
        _HI_CS();
        _LO_DC();
        _LO_CS();
        UNUSED_VARIABLE(spi_transfer(&c, 1));
        _HI_CS();
#endif
    }
}

void ssd1306_begin(uint8_t vccstate, uint8_t i2caddr, bool reset)
{
    _vccstate = vccstate;
    _i2caddr = i2caddr;
    UNUSED_VARIABLE(_i2caddr);

    _width    = WIDTH;
    _height   = HEIGHT;
    cursor_y  = cursor_x    = 0;
    textsize  = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap      = true;
    _cp437    = false;

    _width = WIDTH = SSD1306_LCDWIDTH;
    _height = HEIGHT = SSD1306_LCDHEIGHT;
    rotation  = 0;


    if (reset) {
        // Setup reset pin direction (used by both SPI and I2C)
        _HI_RS();
        // VDD (3.3V) goes high at start, lets just chill for a ms
        nrf_delay_ms(1);
        // bring reset low
        _LO_RS();
        // wait 10ms
        nrf_delay_ms(10);
        // bring out of reset
        _HI_RS();
        // turn on VCC (9V?)
    }

#if defined SSD1306_128_32
    // Init sequence for 128x32 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x1F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x10);
    }
    else {
        ssd1306_command(0x14);
    }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x22);
    }
    else {
        ssd1306_command(0xF1);
    }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
#endif

#if defined SSD1306_128_64
    // Init sequence for 128x64 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x10);
    }
    else {
        ssd1306_command(0x14);
    }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x9F);
    }
    else {
        ssd1306_command(0xCF);
    }
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x22);
    }
    else {
        ssd1306_command(0xF1);
    }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
#endif

#if defined SSD1306_96_16
    // Init sequence for 96x16 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x0F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x00);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x10);
    }
    else {
        ssd1306_command(0x14);
    }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x2);	//ada x12
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x10);
    }
    else {
        ssd1306_command(0xAF);
    }
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) {
        ssd1306_command(0x22);
    }
    else {
        ssd1306_command(0xF1);
    }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
#endif

    ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}



// the most basic function, set a single pixel
void ssd1306_draw_pixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= ssd1306_width()) || (y < 0) || (y >= ssd1306_height()))
        return;

    // check rotation, move pixel around if necessary
    switch (rotation) {
    case 1:
        ssd1306_swap(x, y);
        x = WIDTH - x - 1;
        break;
    case 2:
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        break;
    case 3:
        ssd1306_swap(x, y);
        y = HEIGHT - y - 1;
        break;
    }

    // x is which column
    switch (color) {
    case WHITE:
        buffer[x + (y / 8)*SSD1306_LCDWIDTH] |=  (1 << (y & 7));
        break;
    case BLACK:
        buffer[x + (y / 8)*SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
        break;
    case INVERSE:
        buffer[x + (y / 8)*SSD1306_LCDWIDTH] ^=  (1 << (y & 7));
        break;
    }
}


void ssd1306_invert_display(uint8_t i)
{
    if (i) {
        ssd1306_command(SSD1306_INVERTDISPLAY);
    }
    else {
        ssd1306_command(SSD1306_NORMALDISPLAY);
    }
}


// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_start_scroll_right(uint8_t start, uint8_t stop)
{
    ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X00);
    ssd1306_command(0XFF);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_start_scroll_left(uint8_t start, uint8_t stop)
{
    ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X00);
    ssd1306_command(0XFF);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_start_scroll_diag_right(uint8_t start, uint8_t stop)
{
    ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
    ssd1306_command(0X00);
    ssd1306_command(SSD1306_LCDHEIGHT);
    ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X01);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ssd1306_start_scroll_diag_left(uint8_t start, uint8_t stop)
{
    ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
    ssd1306_command(0X00);
    ssd1306_command(SSD1306_LCDHEIGHT);
    ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
    ssd1306_command(0X00);
    ssd1306_command(start);
    ssd1306_command(0X00);
    ssd1306_command(stop);
    ssd1306_command(0X01);
    ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void ssd1306_stop_scroll(void)
{
    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void ssd1306_dim(bool dim)
{
    uint8_t contrast;

    if (dim) {
        contrast = 0; // Dimmed display
    }
    else {
        if (_vccstate == SSD1306_EXTERNALVCC) {
            contrast = 0x9F;
        }
        else {
            contrast = 0xCF;
        }
    }
    // the range of contrast to too small to be really useful
    // it is useful to dim the display
    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(contrast);
}

void ssd1306_data(uint8_t c)
{
    if (use_i2c) {
        ret_code_t ret;
        uint8_t dta_send[] = {0x40, c};
        ret = nrf_drv_twi_tx(&m_twi_master, _i2caddr, dta_send, 2, false);
        UNUSED_VARIABLE(ret);
    }
    else {
#if ENABLE_SPI
        _HI_CS();
        _HI_DC();
        _LO_CS();
        UNUSED_VARIABLE(spi_transfer(&c, 1));
        _HI_CS();
#endif
    }
}

void ssd1306_display(void)
{
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);   // Column start address (0 = reset)
    ssd1306_command(SSD1306_LCDWIDTH - 1); // Column end address (127 = reset)

    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0); // Page start address (0 = reset)
#if SSD1306_LCDHEIGHT == 64
    ssd1306_command(7); // Page end address
#endif
#if SSD1306_LCDHEIGHT == 32
    ssd1306_command(3); // Page end address
#endif
#if SSD1306_LCDHEIGHT == 16
    ssd1306_command(1); // Page end address
#endif

    if (use_i2c) {
      static uint8_t control = 0x40;
      for (uint16_t i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8); i++) {
        uint8_t tmpBuf[129];
        // SSD1306_SETSTARTLINE
        tmpBuf[0] = control;
        // data
        for (uint8_t j = 0; j < 128; j++) {
          tmpBuf[j + 1] = buffer[i];
          i++;
        }
        i--;

        ret_code_t ret;
        ret = nrf_drv_twi_tx(&m_twi_master, _i2caddr, tmpBuf, sizeof(tmpBuf), false);
        APP_ERROR_CHECK(ret);
      }

    } else {
#if ENABLE_SPI
      _HI_CS();
      _HI_DC();
      _LO_CS();
      for (uint16_t i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8); i++) {
        UNUSED_VARIABLE(spi_transfer(&buffer[i], 1));
      }
      _HI_CS();
#endif
    }
}

// clear everything
void ssd1306_clear_display(void)
{
    memset(buffer, 0, (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8));
}



void ssd1306_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    bool __swap = false;
    switch (rotation) {
    case 0:
        // 0 degree rotation, do nothing
        break;
    case 1:
        // 90 degree rotation, swap x & y for rotation, then invert x
        __swap = true;
        ssd1306_swap(x, y);
        x = WIDTH - x - 1;
        break;
    case 2:
        // 180 degree rotation, invert x and y - then shift y around for height.
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        x -= (w - 1);
        break;
    case 3:
        // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
        __swap = true;
        ssd1306_swap(x, y);
        y = HEIGHT - y - 1;
        y -= (w - 1);
        break;
    }

    if (__swap) {
        ssd1306_draw_fast_vline_internal(x, y, w, color);
    }
    else {
        ssd1306_draw_fast_hline_internal(x, y, w, color);
    }
}

void ssd1306_draw_fast_hline_internal(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    // Do bounds/limit checks
    if (y < 0 || y >= HEIGHT) {
        return;
    }

    // make sure we don't try to draw below 0
    if (x < 0) {
        w += x;
        x = 0;
    }

    // make sure we don't go off the edge of the display
    if ( (x + w) > WIDTH) {
        w = (WIDTH - x);
    }

    // if our width is now negative, punt
    if (w <= 0) {
        return;
    }

    // set up the pointer for  movement through the buffer
    register uint8_t *pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    // and offset x columns in
    pBuf += x;

    register uint8_t mask = 1 << (y & 7);

    switch (color) {
    case WHITE:
        while (w--) {
            *pBuf++ |= mask;
        };
        break;
    case BLACK:
        mask = ~mask;
        while (w--) {
            *pBuf++ &= mask;
        };
        break;
    case INVERSE:
        while (w--) {
            *pBuf++ ^= mask;
        };
        break;
    }
}

void ssd1306_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    bool __swap = false;
    switch (rotation) {
    case 0:
        break;
    case 1:
        // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
        __swap = true;
        ssd1306_swap(x, y);
        x = WIDTH - x - 1;
        x -= (h - 1);
        break;
    case 2:
        // 180 degree rotation, invert x and y - then shift y around for height.
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        y -= (h - 1);
        break;
    case 3:
        // 270 degree rotation, swap x & y for rotation, then invert y
        __swap = true;
        ssd1306_swap(x, y);
        y = HEIGHT - y - 1;
        break;
    }

    if (__swap) {
        ssd1306_draw_fast_hline_internal(x, y, h, color);
    }
    else {
        ssd1306_draw_fast_vline_internal(x, y, h, color);
    }
}


void ssd1306_draw_fast_vline_internal(int16_t x, int16_t __y, int16_t __h, uint16_t color)
{

    // do nothing if we're off the left or right side of the screen
    if (x < 0 || x >= WIDTH) {
        return;
    }

    // make sure we don't try to draw below 0
    if (__y < 0) {
        // __y is negative, this will subtract enough from __h to account for __y being 0
        __h += __y;
        __y = 0;

    }

    // make sure we don't go past the height of the display
    if ( (__y + __h) > HEIGHT) {
        __h = (HEIGHT - __y);
    }

    // if our height is now negative, punt
    if (__h <= 0) {
        return;
    }

    // this display doesn't need ints for coordinates, use local byte registers for faster juggling
    register uint8_t y = __y;
    register uint8_t h = __h;


    // set up the pointer for fast movement through the buffer
    register uint8_t *pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    // and offset x columns in
    pBuf += x;

    // do the first partial byte, if necessary - this requires some masking
    register uint8_t mod = (y & 7);
    if (mod) {
        // mask off the high n bits we want to set
        mod = 8 - mod;

        // note - lookup table results in a nearly 10% performance improvement in fill* functions
        // register uint8_t mask = ~(0xFF >> (mod));
        static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
        register uint8_t mask = premask[mod];

        // adjust the mask if we're not going to reach the end of this byte
        if ( h < mod) {
            mask &= (0XFF >> (mod - h));
        }

        switch (color) {
        case WHITE:
            *pBuf |=  mask;
            break;
        case BLACK:
            *pBuf &= ~mask;
            break;
        case INVERSE:
            *pBuf ^=  mask;
            break;
        }

        // fast exit if we're done here!
        if (h < mod) {
            return;
        }

        h -= mod;

        pBuf += SSD1306_LCDWIDTH;
    }


    // write solid bytes while we can - effectively doing 8 rows at a time
    if (h >= 8) {
        if (color == INVERSE)  {          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
            do  {
                *pBuf = ~(*pBuf);

                // adjust the buffer forward 8 rows worth of data
                pBuf += SSD1306_LCDWIDTH;

                // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                h -= 8;
            }
            while (h >= 8);
        }
        else {
            // store a local value to work with
            register uint8_t val = (color == WHITE) ? 255 : 0;

            do  {
                // write our value in
                *pBuf = val;

                // adjust the buffer forward 8 rows worth of data
                pBuf += SSD1306_LCDWIDTH;

                // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                h -= 8;
            }
            while (h >= 8);
        }
    }

    // now do the final partial byte, if necessary
    if (h) {
        mod = h & 7;
        // this time we want to mask the low bits of the byte, vs the high bits we did above
        // register uint8_t mask = (1 << mod) - 1;
        // note - lookup table results in a nearly 10% performance improvement in fill* functions
        static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
        register uint8_t mask = postmask[mod];
        switch (color) {
        case WHITE:
            *pBuf |=  mask;
            break;
        case BLACK:
            *pBuf &= ~mask;
            break;
        case INVERSE:
            *pBuf ^=  mask;
            break;
        }
    }
}


// Draw a circle outline
void ssd1306_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ssd1306_draw_pixel(x0  , y0 + r, color);
    ssd1306_draw_pixel(x0  , y0 - r, color);
    ssd1306_draw_pixel(x0 + r, y0  , color);
    ssd1306_draw_pixel(x0 - r, y0  , color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ssd1306_draw_pixel(x0 + x, y0 + y, color);
        ssd1306_draw_pixel(x0 - x, y0 + y, color);
        ssd1306_draw_pixel(x0 + x, y0 - y, color);
        ssd1306_draw_pixel(x0 - x, y0 - y, color);
        ssd1306_draw_pixel(x0 + y, y0 + x, color);
        ssd1306_draw_pixel(x0 - y, y0 + x, color);
        ssd1306_draw_pixel(x0 + y, y0 - x, color);
        ssd1306_draw_pixel(x0 - y, y0 - x, color);
    }
}

void ssd1306_draw_circle_helper(int16_t x0, int16_t y0,
                                int16_t r, uint8_t cornername, uint16_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            ssd1306_draw_pixel(x0 + x, y0 + y, color);
            ssd1306_draw_pixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            ssd1306_draw_pixel(x0 + x, y0 - y, color);
            ssd1306_draw_pixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            ssd1306_draw_pixel(x0 - y, y0 + x, color);
            ssd1306_draw_pixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            ssd1306_draw_pixel(x0 - y, y0 - x, color);
            ssd1306_draw_pixel(x0 - x, y0 - y, color);
        }
    }
}

void ssd1306_fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    ssd1306_draw_fast_vline(x0, y0 - r, 2 * r + 1, color);
    ssd1306_fill_circle_helper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void ssd1306_fill_circle_helper(int16_t x0, int16_t y0, int16_t r,
                                uint8_t cornername, int16_t delta, uint16_t color)
{

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) {
            ssd1306_draw_fast_vline(x0 + x, y0 - y, 2 * y + 1 + delta, color);
            ssd1306_draw_fast_vline(x0 + y, y0 - x, 2 * x + 1 + delta, color);
        }
        if (cornername & 0x2) {
            ssd1306_draw_fast_vline(x0 - x, y0 - y, 2 * y + 1 + delta, color);
            ssd1306_draw_fast_vline(x0 - y, y0 - x, 2 * x + 1 + delta, color);
        }
    }
}

// Bresenham's algorithm - thx wikpedia
void ssd1306_draw_line(int16_t x0, int16_t y0,
                       int16_t x1, int16_t y1,
                       uint16_t color)
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        adagfxswap(x0, y0);
        adagfxswap(x1, y1);
    }

    if (x0 > x1) {
        adagfxswap(x0, x1);
        adagfxswap(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    }
    else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            ssd1306_draw_pixel(y0, x0, color);
        }
        else {
            ssd1306_draw_pixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

// Draw a rectangle
void ssd1306_draw_rect(int16_t x, int16_t y,
                       int16_t w, int16_t h,
                       uint16_t color)
{
    ssd1306_draw_fast_hline(x, y, w, color);
    ssd1306_draw_fast_hline(x, y + h - 1, w, color);
    ssd1306_draw_fast_vline(x, y, h, color);
    ssd1306_draw_fast_vline(x + w - 1, y, h, color);
}

#if 0
void Adafruit_GFX::drawFastVLine(int16_t x, int16_t y,
                                 int16_t h, uint16_t color)
{
    // Update in subclasses if desired!
    drawLine(x, y, x, y + h - 1, color);
}

void Adafruit_GFX::drawFastHLine(int16_t x, int16_t y,
                                 int16_t w, uint16_t color)
{
    // Update in subclasses if desired!
    drawLine(x, y, x + w - 1, y, color);
}
#endif

void ssd1306_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    // Update in subclasses if desired!
    for (int16_t i = x; i < x + w; i++) {
        ssd1306_draw_fast_vline(i, y, h, color);
    }
}

void ssd1306_fill_screen(uint16_t color)
{
    ssd1306_fill_rect(0, 0, _width, _height, color);
}

// Draw a rounded rectangle
void ssd1306_draw_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    // smarter version
    ssd1306_draw_fast_hline(x + r  , y    , w - 2 * r, color); // Top
    ssd1306_draw_fast_hline(x + r  , y + h - 1, w - 2 * r, color); // Bottom
    ssd1306_draw_fast_vline(x    , y + r  , h - 2 * r, color); // Left
    ssd1306_draw_fast_vline(x + w - 1, y + r  , h - 2 * r, color); // Right
    // draw four corners
    ssd1306_draw_circle_helper(x + r    , y + r    , r, 1, color);
    ssd1306_draw_circle_helper(x + w - r - 1, y + r    , r, 2, color);
    ssd1306_draw_circle_helper(x + w - r - 1, y + h - r - 1, r, 4, color);
    ssd1306_draw_circle_helper(x + r    , y + h - r - 1, r, 8, color);
}

// Fill a rounded rectangle
void ssd1306_fill_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    // smarter version
    ssd1306_fill_rect(x + r, y, w - 2 * r, h, color);

    // draw four corners
    ssd1306_fill_circle_helper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    ssd1306_fill_circle_helper(x + r    , y + r, r, 2, h - 2 * r - 1, color);
}

// Draw a triangle
void ssd1306_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    ssd1306_draw_line(x0, y0, x1, y1, color);
    ssd1306_draw_line(x1, y1, x2, y2, color);
    ssd1306_draw_line(x2, y2, x0, y0, color);
}

// Fill a triangle
void ssd1306_fill_triangle( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        adagfxswap(y0, y1);
        adagfxswap(x0, x1);
    }
    if (y1 > y2) {
        adagfxswap(y2, y1);
        adagfxswap(x2, x1);
    }
    if (y0 > y1) {
        adagfxswap(y0, y1);
        adagfxswap(x0, x1);
    }

    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)      a = x1;
        else if (x1 > b) b = x1;
        if (x2 < a)      a = x2;
        else if (x2 > b) b = x2;
        ssd1306_draw_fast_hline(a, y0, b - a + 1, color);
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2) last = y1;  // Include y1 scanline
    else         last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b) adagfxswap(a, b);
        ssd1306_draw_fast_hline(a, y, b - a + 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b) adagfxswap(a, b);
        ssd1306_draw_fast_hline(a, y, b - a + 1, color);
    }
}


void ssd1306_draw_bitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
    int16_t i, j, byteWidth = (w + 7) / 8;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++ ) {
            if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                ssd1306_draw_pixel(x + i, y + j, color);
            }
        }
    }
}

// Draw a 1-bit color bitmap at the specified x, y position from the
// provided bitmap buffer (must be PROGMEM memory) using color as the
// foreground color and bg as the background color.
void ssd1306_draw_bitmap_bg(int16_t x, int16_t y,
                            const uint8_t *bitmap, int16_t w, int16_t h,
                            uint16_t color, uint16_t bg)
{
    int16_t i, j, byteWidth = (w + 7) / 8;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++ ) {
            if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                ssd1306_draw_pixel(x + i, y + j, color);
            }
            else {
                ssd1306_draw_pixel(x + i, y + j, bg);
            }
        }
    }
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void ssd1306_draw_xbitmap(int16_t x, int16_t y,
                          const uint8_t *bitmap, int16_t w, int16_t h,
                          uint16_t color)
{

    int16_t i, j, byteWidth = (w + 7) / 8;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++ ) {
            if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
                ssd1306_draw_pixel(x + i, y + j, color);
            }
        }
    }
}

size_t ssd1306_write(uint8_t c)
{
    if (c == '\n') {
        cursor_y += textsize * 8;
        cursor_x  = 0;
    }
    else if (c == '\r') {
        // skip em
    }
    else {
        ssd1306_draw_char(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
        cursor_x += textsize * 6;
        if (wrap && (cursor_x > (_width - textsize * 6))) {
            cursor_y += textsize * 8;
            cursor_x = 0;
        }
    }

    return 1;
}

// Draw a character
void ssd1306_draw_char(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size)
{

    if ((x >= _width)            || // Clip right
            (y >= _height)           || // Clip bottom
            ((x + 6 * size - 1) < 0) || // Clip left
            ((y + 8 * size - 1) < 0))   // Clip top
        return;

    if (!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

    for (int8_t i = 0; i < 6; i++ ) {
        uint8_t line;
        if (i == 5)
            line = 0x0;
        else
            line = pgm_read_byte(font + (c * 5) + i);
        for (int8_t j = 0; j < 8; j++) {
            if (line & 0x1) {
                if (size == 1) // default size
                    ssd1306_draw_pixel(x + i, y + j, color);
                else {  // big size
                    ssd1306_fill_rect(x + (i * size), y + (j * size), size, size, color);
                }
            }
            else if (bg != color) {
                if (size == 1) // default size
                    ssd1306_draw_pixel(x + i, y + j, bg);
                else {  // big size
                    ssd1306_fill_rect(x + i * size, y + j * size, size, size, bg);
                }
            }
            line >>= 1;
        }
    }
}

void ssd1306_set_cursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
}

int16_t ssd1306_get_cursor_x(void)
{
    return cursor_x;
}

int16_t ssd1306_get_cursor_y(void)
{
    return cursor_y;
}

void ssd1306_set_textsize(uint8_t s)
{
    textsize = (s > 0) ? s : 1;
}

uint8_t ssd1306_get_textsize(void)
{
    return textsize;
}


void ssd1306_set_textcolor(uint16_t c)
{
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
}

void ssd1306_set_textcolor_bg(uint16_t c, uint16_t b)
{
    textcolor   = c;
    textbgcolor = b;
}

void ssd1306_set_textwrap(bool w)
{
    wrap = w;
}

uint8_t ssd1306_get_rotation(void)
{
    return rotation;
}

void ssd1306_set_rotation(uint8_t x)
{
    rotation = (x & 3);
    switch (rotation) {
    case 0:
    case 2:
        _width  = WIDTH;
        _height = HEIGHT;
        break;
    case 1:
    case 3:
        _width  = HEIGHT;
        _height = WIDTH;
        break;
    }
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void ssd1306_cp437(bool x)
{
    _cp437 = x;
}


void ssd1306_putstring(char* buffer)
{
    while (*buffer) {
        ssd1306_write((uint8_t)*buffer);
        buffer++;
    }
}

void ssd1306_puts(char* buffer)
{
    ssd1306_putstring(buffer);
    ssd1306_write('\n');
}

int ssd1306_printf_length(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    int length = vsnprintf(NULL, 0, format, argptr);
    va_end(argptr);
    return length;
}

void ssd1306_printf(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    int length = vsnprintf(NULL, 0, format, argptr);
    char *str = calloc(1, length+1);
    vsnprintf(str, length+1, format, argptr);
    ssd1306_putstring(str);
    free(str);
    va_end(argptr);
}
