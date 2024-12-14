/*
 * lcd1602.c
 *
 *  Created on: Nov 29, 2024
 *      Author: denis
 */
#include "main.h"
#include "lcd1602.h"
#include "lcd_data_transport.h"


/** @brief Позиционирует курсор
 *  @details рассчитано на 2 строки
 *  @param [in] row № строки (начинается с 0)
 *  @param [in] col № колонки (начинается с 0)
 */
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address;

    // Определяем адрес DDRAM
    if (row == 0) {
        address = 0x00 + col;  // Первая строка
    } else if (row == 1) {
        address = 0x40 + col;  // Вторая строка
    }

    // Отправляем команду установки адреса DDRAM
    LCD_SendCommand(0x80 | address);
}

/** @brief Отправляет строку
 *  @param [in] str указатель на строку
 *  @param [in] size размер строки в байтах
 *  @return None
 */
void LCD_SendString(char *str, uint8_t size)
{
	uint8_t cnt = 0;
	while(*str && cnt < size)
	{
		LCD_SendData(*str++);
		cnt ++;
	}
}

#if LCD_DATA_WIDTH == LCD_DATA_WIDTH_BYTE
/** @brief Инициализация дисплея в 8битном режиме
 */
static void s_lcd_init_8bit (void)
{
	HAL_Delay(15); 				   // Задержка после подачи питания
	LCD_SendCommand(0b00110000);   // 8ми битный интерфейс
	HAL_Delay(5);
	LCD_SendCommand(0b00110000);   // 8ми битный интерфейс
	HAL_Delay(1);
	LCD_SendCommand(0b00111000);   // 8ми битный интерфейс, две строки
	HAL_Delay(1);
	LCD_SendCommand(0b00001000);   // Display Off
	LCD_SendCommand(0b00000010);   // установка курсора в начале строки
	LCD_SendCommand(0b00001100);   // нормальный режим работы, выкл курсор
	LCD_SendCommand(0b00000001);   // очистка дисплея
	LCD_SendCommand(0b00000010);   // режим ввода
	HAL_Delay(10);
}

#elif LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE

/** @brief инициализировать LCD в 4 битном режиме
 *  @details
 *  	Чуть-чуть сложнее. Сначала передать биты при 8 битном режиме
 *  	Но можно передавать только старшие биты,
 *  	поэтому инициализацию в 4 битном режиме можно передать только в
 *  	виде 0b00000010, т.е., на самом деле, чип HD44780
 *  	Получит сначала 0b0000000, а потом 0b00100000
 *  	Теперь, 4 битный режим включён и можно передавать байты, как есть
 *  	без учёта того, что команда передаётся полубайтами
 *  @return None
 */
static void s_lcd_init_4bit (void)
{
	HAL_Delay(25);                 // Задержка после подачи питания
	LCD_SendCommand(0b00110011);   // 8 битный интерфейс. Повторяется два раза
	HAL_Delay(5);
	// Включить 4 битный режим (поскольку передаётся на 4 старших
	// бита, первые 0000, далее, забрасываем режим 4 бита
	LCD_SendCommand(0b00000010);
	HAL_Delay(5);
	// Теперь, можно передавать полубайтами, байт, как есть.
	LCD_SendCommand(0b00101000);   // Включить 2 строки, 4 бита
	LCD_SendCommand(0b00001000);   // Выключить дисплей
	LCD_SendCommand(0b00000010);   // установка курсора в начале строки
	LCD_SendCommand(0b00001100);   // нормальный режим работы, выкл курсор
	LCD_SendCommand(0b00000001);   // очистка дисплея
	LCD_SendCommand(0b00000010);   // режим ввода
	HAL_Delay(10);
}
#endif

void LCD_Init(void)
{
	LCD_TransportInit();
#if LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE
	s_lcd_init_4bit ();
#elif LCD_DATA_WIDTH == LCD_DATA_WIDTH_BYTE
	s_lcd_init_8bit ();
#endif
}

void LCD_Clear (void)
{
	LCD_SendCommand(0b00000001);
}


