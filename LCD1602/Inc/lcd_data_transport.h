/*
 * lcd_data_transport.h
 *
 *  Created on: Dec 2, 2024
 *      Author: denis
 */
#include <stdint.h>

#ifndef INC_LCD_DATA_TRANSPORT_H_
#define INC_LCD_DATA_TRANSPORT_H_

#define START_STROB 1                   ///?> Строб для запуска чтения данных логическим анализатором

#define LCD_DATA_WIDTH_8BIT           0 ///?> Ширина данных 8 бит (выбрать 1 из двух)
#define LCD_DATA_WIDTH_4BIT           1 ///?> Ширина данных 4 бита (выбрать 1 из двух)

#define LCD_DATA_TRANSPORT_GPIO       0 ///?> Транспортный протокол GPIO (прямая передача данных через GPIO-порты)
#define LCD_DATA_TRANSPORT_74HC595    0 ///?> Транспортный протокол через 74HC595
#define LCD_DATA_TRANSPORT_PCF8574T   1 ///?> Транспортный протокол через I2C PCF8574T

#define LCD_DATA_WIDTH_BYTE           1 ///?> Ширина данных 8 бит (байт)
#define LCD_DATA_WIDTH_HALF_BYTE      2 ///?> Ширина данных 4 бита (полубайт)

#if LCD_DATA_WIDTH_8BIT != 0
#undef LCD_DATA_WIDTH
#define LCD_DATA_WIDTH LCD_DATA_WIDTH_BYTE

#elif LCD_DATA_WIDTH_4BIT != 0
#undef LCD_DATA_WIDTH
#define LCD_DATA_WIDTH LCD_DATA_WIDTH_HALF_BYTE
#endif

#define LCD_DATA_GPIO                1 ///?> Транспорт GPIO
#define LCD_DATA_74HC595             2 ///?> Транспорт 74HC595
#define LCD_DATA_PCF8574T            3 ///?> Транспорт PCF8574T

#if LCD_DATA_TRANSPORT_GPIO != 0
#undef  LCD_DATA_TRANSPORT
#define LCD_DATA_TRANSPORT LCD_DATA_GPIO
#elif LCD_DATA_TRANSPORT_74HC595 != 0
#undef  LCD_DATA_TRANSPORT
#define LCD_DATA_TRANSPORT LCD_DATA_74HC595
#elif LCD_DATA_TRANSPORT_PCF8574T != 0
#undef  LCD_DATA_TRANSPORT
#define LCD_DATA_TRANSPORT LCD_DATA_PCF8574T
#endif

void LCD_TransportInit (void);
void LCD_SendCommand   (uint8_t cmd);
void LCD_SendData      (uint8_t data);

#endif /* INC_LCD_DATA_TRANSPORT_H_ */
