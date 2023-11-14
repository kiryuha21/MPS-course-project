/*
 * ST773.c
 *
 */


#include "ST7735.h"
#include "stdlib.h"

#define TFT_BL_H()  HAL_GPIO_WritePin(ST7735_BL_GPIO_Port, ST7735_BL_Pin, GPIO_PIN_SET)
#define TFT_BL_L()  HAL_GPIO_WritePin(ST7735_BL_GPIO_Port, ST7735_BL_Pin, GPIO_PIN_RESET)
#define TFT_CS_H()  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET)
#define TFT_CS_L()  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET)
#define TFT_DC_D()  HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET)
#define TFT_DC_C()  HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET)
#define TFT_RES_H() HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET)
#define TFT_RES_L() HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET)

#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))
#define SWAP_INT16_T(a, b) { int16_t t = a; a = b; b = t; }
#define DELAY 0x80

#if defined(ST7735_1_8_DEFAULT_ORIENTATION) || defined(ST7735S_1_8_DEFAULT_ORIENTATION)
static uint8_t _data_rotation[4] = { ST7735_MADCTL_MX, ST7735_MADCTL_MY, ST7735_MADCTL_MV, ST7735_MADCTL_RGB };
#endif

#if defined(ST7735_1_44_DEFAULT_ORIENTATION) || defined(ST7735_MINI_DEFAULT_ORIENTATION)
static uint8_t _data_rotation[4] = { ST7735_MADCTL_MX, ST7735_MADCTL_MY, ST7735_MADCTL_MV, ST7735_MADCTL_BGR };
#endif

static uint8_t _value_rotation = 0;
static int16_t _height = ST7735_HEIGHT, _width = ST7735_WIDTH;
static uint8_t _xstart = ST7735_XSTART, _ystart = ST7735_YSTART;

// based on Adafruit ST7735 library for Arduino
static const uint8_t
init_cmds1[] = {                // Init for 7735R, part 1 (red or green tab)
      15,                       // 15 commands in list:
      ST7735_SWRESET, DELAY,    //  1: Software reset, 0 args, w/delay
      150,                      //     150 ms delay
      ST7735_SLPOUT, DELAY,   //  2: Out of sleep mode, 0 args, w/delay
      255,                      //     500 ms delay
      ST7735_FRMCTR1, 3,    //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,         //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
      ST7735_FRMCTR2, 3,      //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,         //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
      ST7735_FRMCTR3, 6,      //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,         //     Dot inversion mode
      0x01, 0x2C, 0x2D,         //     Line inversion mode
      ST7735_INVCTR, 1,     //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                     //     No inversion
      ST7735_PWCTR1, 3,     //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                     //     -4.6V
      0x84,                     //     AUTO mode
      ST7735_PWCTR2, 1,     //  8: Power control, 1 arg, no delay:
      0xC5,                     //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
      ST7735_PWCTR3, 2,     //  9: Power control, 2 args, no delay:
      0x0A,                     //     Opamp current small
      0x00,                     //     Boost frequency
      ST7735_PWCTR4, 2,     // 10: Power control, 2 args, no delay:
      0x8A,                     //     BCLK/2, Opamp current small & Medium low
      0x2A,
      ST7735_PWCTR5, 2,     // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
      ST7735_VMCTR1, 1,     // 12: Power control, 1 arg, no delay:
      0x0E,
      ST7735_INVOFF, 0,     // 13: Don't invert display, no args, no delay
      ST7735_MADCTL, 1,     // 14: Memory access control (directions), 1 arg:
      ST7735_DATA_ROTATION,     //     row addr/col addr, bottom to top refresh
      ST7735_COLMOD, 1,     // 15: set color mode, 1 arg, no delay:
      0x05},                  //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                  //  2 commands in list:
    ST7735_CASET, 4,    //  1: Column addr set, 4 args, no delay:
    0x00, 0x00,         //     XSTART = 0
    0x00, 0x7F,         //     XEND = 127
    ST7735_RASET, 4,    //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,         //     XSTART = 0
    0x00, 0x7F },       //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
init_cmds2[] = {          // Init for 7735S, part 2 (160x80 display)
    3,                          //  3 commands in list:
    ST7735_CASET, 4,        //  1: Column addr set, 4 args, no delay:
    0x00, 0x00,               //     XSTART = 0
    0x00, 0x4F,               //     XEND = 79
    ST7735_RASET, 4,        //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,               //     XSTART = 0
    0x00, 0x9F ,              //     XEND = 159
    ST7735_INVON, 0 },        //  3: Invert colors
#endif

init_cmds3[] = {                // Init for 7735R, part 3 (red or green tab)
    4,                          //  4 commands in list:
    ST7735_GMCTRP1, 16,     //  1: Magical unicorn dust, 16 args, no delay:
    0x02, 0x1c, 0x07, 0x12,
    0x37, 0x32, 0x29, 0x2d,
    0x29, 0x25, 0x2B, 0x39,
    0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16,     //  2: Sparkles and rainbows, 16 args, no delay:
    0x03, 0x1d, 0x07, 0x06,
    0x2E, 0x2C, 0x29, 0x2D,
    0x2E, 0x2E, 0x37, 0x3F,
    0x00, 0x00, 0x02, 0x10,
    ST7735_NORON, DELAY,    //  3: Normal display on, no args, w/delay
    10,                       //     10 ms delay
    ST7735_DISPON, DELAY,     //  4: Main screen turn on, no args w/delay
    100 };                    //     100 ms delay

static void ST7735_GPIO_Init(void);
static void ST7735_WriteCommand(uint8_t cmd);
static void ST7735_WriteData(uint8_t* buff, size_t buff_size);
static void ST7735_ExecuteCommandList(const uint8_t *addr);
static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);

static void ST7735_GPIO_Init(void)
{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOB, ST7735_RES_Pin|ST7735_DC_Pin|ST7735_CS_Pin|ST7735_BL_Pin, GPIO_PIN_RESET);
//
//  /*Configure GPIO pins : ST7735_RES_Pin ST7735_DC_Pin ST7735_CS_Pin ST7735_BL_Pin */
//  GPIO_InitStruct.Pin = ST7735_RES_Pin|ST7735_DC_Pin|ST7735_CS_Pin|ST7735_BL_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void ST7735_Reset()
{
  TFT_RES_L();
  HAL_Delay(20);
  TFT_RES_H();
}

static void ST7735_WriteCommand(uint8_t cmd)
{
  TFT_DC_C();
#ifdef USE_SPI_DMA
  HAL_SPI_Transmit_DMA(&ST7735_SPI_PORT, &cmd, sizeof(cmd));
  //while(hspi1.State == HAL_SPI_STATE_BUSY_TX);
#else
  HAL_SPI_Transmit(&ST7735_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
#endif
}

static void ST7735_WriteData(uint8_t* buff, size_t buff_size)
{
  TFT_DC_D();
#ifdef USE_SPI_DMA
  HAL_SPI_Transmit_DMA(&ST7735_SPI_PORT, buff, buff_size);
  while(hspi1.State == HAL_SPI_STATE_BUSY_TX);

#else
  HAL_SPI_Transmit(&ST7735_SPI_PORT, buff, buff_size, HAL_MAX_DELAY);
#endif
}

static void ST7735_ExecuteCommandList(const uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--)
    {
      uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs)
        {
            ST7735_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms)
        {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}

static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + _xstart, 0x00, x1 + _xstart };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_RASET);
    data[1] = y0 + _ystart;
    data[3] = y1 + _ystart;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_RAMWR);
}

static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    ST7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++)
    {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++)
        {
            if((b << j) & 0x8000)
            {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
            else
            {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
}

void ST7735_Init()
{
  ST7735_GPIO_Init();
  TFT_CS_L();
    ST7735_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
    ST7735_ExecuteCommandList(init_cmds3);
    TFT_CS_H();
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if((x >= _width) || (y >= _height))
        return;

    TFT_CS_L();

    ST7735_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    TFT_CS_H();
}

void ST7735_DrawString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
  TFT_CS_L();

    while(*str)
    {
        if(x + font.width >= _width)
        {
            x = 0;
            y += font.height;
            if(y + font.height >= _height)
            {
                break;
            }

            if(*str == ' ')
            {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
    TFT_CS_H();
}

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    // clipping
    if((x >= _width) || (y >= _height)) return;
    if((x + w - 1) >= _width) w = _width - x;
    if((y + h - 1) >= _height) h = _height - y;

    TFT_CS_L();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    TFT_DC_D();
    for(y = h; y > 0; y--)
    {
        for(x = w; x > 0; x--)
        {
#ifdef USE_SPI_DMA
          HAL_SPI_Transmit_DMA(&ST7735_SPI_PORT, data, sizeof(data));
          //while(hspi1.State == HAL_SPI_STATE_BUSY_TX);
#else
          HAL_SPI_Transmit(&ST7735_SPI_PORT, data, sizeof(data), HAL_MAX_DELAY);
#endif
        }
    }
    TFT_CS_H();
}

void ST7735_FillScreen(uint16_t color)
{
    ST7735_FillRectangle(0, 0, _width, _height, color);
}

void ST7735_DrawTouchGFX(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data)
{
    if((x >= _width) || (y >= _height)) return;
    if((x + w - 1) >= _width) return;
    if((y + h - 1) >= _height) return;

    TFT_CS_L();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint32_t size = w * h;
    uint8_t colorBytes[size][2];

    for (uint32_t i = 0; i < size; i++)
  {
    colorBytes[i][0] = (*data & 0xFF00) >> 8;
    colorBytes[i][1] = *data & 0x00FF;
    data++;
  }

    TFT_DC_D();
  ST7735_WriteData((uint8_t*) &colorBytes, size * 2);
    TFT_CS_H();
}

void ST7735_InvertColors(bool invert)
{
  TFT_CS_L();
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    TFT_CS_H();
}

void ST7735_Backlight_On(void)
{
  TFT_BL_H();
}

void ST7735_Backlight_Off(void)
{
  TFT_BL_L();
}

/***************************************************************************************
** Function name:           setRotation
** Description:             rotate the screen orientation m = 0-3
***************************************************************************************/
void ST7735_SetRotation()
{
  TFT_CS_L();

  ST7735_WriteCommand(ST7735_MADCTL);

  uint8_t d_r = _data_rotation[3];
  ST7735_WriteData(&d_r, sizeof(d_r));
  _width  = ST7735_WIDTH;
  _height = ST7735_HEIGHT;
  _xstart = ST7735_XSTART;
  _ystart = ST7735_YSTART;

  TFT_CS_H();
}

uint8_t ST7735_GetRotation(void)
{
  return _value_rotation;
}

int16_t ST7735_GetHeight(void)
{
  return _height;
}

int16_t ST7735_GetWidth(void)
{
  return _width;
}

void format_buffer(char* buffer, size_t line_width) {
	char res[100] = {'\0'};

	size_t shift = 0;
	size_t len = strlen(buffer);
	for (size_t i = 0; i < len; ++i) {
		if (buffer[i] != ' ') {
			res[i + shift] = buffer[i];
			continue;
		}

		size_t spaces_amount = line_width - ((i + shift) % line_width);
		for (size_t j = 0; j < spaces_amount; ++j) {
			res[i + j + shift] = ' ';
		}
		shift += spaces_amount - 1;
	}

	strcpy(buffer, res);
}
