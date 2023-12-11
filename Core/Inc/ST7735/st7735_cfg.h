#ifndef ST7735_CFG_H_
#define ST7735_CFG_H_

#include "main.h"
#define ST7735_SPI_PORT hspi1

#define ST7735S_1_8_DEFAULT_ORIENTATION   // WaveShare ST7735S-based 1.8" display, default orientation

//Port and pin connected signal 'RES' (reset) ST7735 display
#ifndef ST7735_RES_Pin
#define ST7735_RES_Pin      GPIO_PIN_10
#endif
#ifndef ST7735_RES_GPIO_Port
#define ST7735_RES_GPIO_Port  GPIOA
#endif
//Port and pin connected signal 'DC' (data or command) ST7735 display
#ifndef ST7735_DC_Pin
#define ST7735_DC_Pin       GPIO_PIN_11
#endif
#ifndef ST7735_DC_GPIO_Port
#define ST7735_DC_GPIO_Port   GPIOA
#endif
//Port and pin connected signal 'CS' (chip select) ST7735 display
#ifndef ST7735_CS_Pin
#define ST7735_CS_Pin       GPIO_PIN_12
#endif
#ifndef ST7735_CS_GPIO_Port
#define ST7735_CS_GPIO_Port   GPIOA
#endif

#endif /* ST7735_CFG_H_ */
