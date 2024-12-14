/*
 * lcd1602.h
 *
 *  Created on: Nov 29, 2024
 *      Author: denis
 */
#include <stdint.h>

#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_

void LCD_SetCursor    (uint8_t row, uint8_t col);
void LCD_Init         (void);
void LCD_SetCursor    (uint8_t row, uint8_t col);
void LCD_SendString   (char *str, uint8_t size);
void LCD_Clear        (void);

#endif /* INC_LCD1602_H_ */
