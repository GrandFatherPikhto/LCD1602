/*
 * LCD_DATA_TRANSPORT.c
 *
 *  Created on: Dec 2, 2024
 *      Author: denis
 */
#include "lcd_data_transport.h"
#include "gpio.h"

#define STUPID_DELAY 400

/// Объявления локальных статических функций
static void s_send_data       (uint8_t data);   ///?> Отправка байта данных LCD1602  (+RS Строб)
static void s_send_command    (uint8_t data);   ///?> Отправка байта команды LCD1602
static void s_stupid_delay    (uint32_t delay); ///?> Ожидание в цикле
static void s_transport_init  (void);           ///?> Инициализация транспорта, если нужно

/** @brief "Тупое" ожидание в цикле
 *  @note
 *  	Использовать в этом случае синхронизацию по таймеру не вижу смысла
 *  	Хотя, самым перфектным решением было бы пустить данные через DMA на порт
 *  @return None
 */
static inline void s_stupid_delay   (uint32_t delay)
{
	while(delay)
		delay --;
}


/** @brief Предварительная инициализация
 *	@note
 *		В любой реализации транспорта должна присуствовать хотя бы заглушка этой функции
 *	@return None
 */
void LCD_TransportInit (void)
{
	s_transport_init ();
}

/** @brief Отправляет байт, как команду (Линия RS не стробируется)
 *  @note
 *  	Пины:
 *  	RS_Pin -- не стробируется
 *  	E_Pin  -- стробируется
 *  	s_send_command должна быть определена в соответствующем транспорте
 *  @return None
 */
void LCD_SendCommand(uint8_t data)
{
	s_send_command (data);
	HAL_Delay(1);
}

/** @brief Отправляет байт, как данные (Линия RS стробируется)
 *  @note
 *  	Пины:
 *  	RS_Pin -- стробируется
 *  	E_Pin  -- стробируется
 *  	s_send_data должна быть определена в соответствующем транспорте
 *  @return None
 */
void LCD_SendData (uint8_t data)
{
	s_send_data (data);
	HAL_Delay(1);
}


#if (LCD_DATA_TRANSPORT == LCD_DATA_GPIO) ///?> Блок управления при помощи GPIO. Для полубайта и байта
#define GPIO_PORT GPIOD

static void s_set_gpio       (uint8_t data, uint32_t add);
static void s_transport_byte (uint8_t data, uint32_t add);
static void s_reset_gpio     (uint32_t add);

/** @brief Сбрасывает порты GPIO
 *  @note
 *  	Сбрасывает 4 порта, если выбран режим полубайта
 *  	Сбрасывает 8 портов, если выбран режим байта
 */
static void s_reset_gpio(uint32_t add)
{
#if	(LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE)
    // Очистка всех используемых пинов (сброс)
    GPIO_PORT->BSRR = (D4_Pin | D5_Pin | D6_Pin | D7_Pin | add) << 0x10;
#elif (LCD_DATA_WIDTH == LCD_DATA_WIDTH_BYTE)
    GPIO_PORT->BSRR = (D0_Pin | D1_Pin | D2_Pin | D3_Pin | D4_Pin | D5_Pin | D6_Pin | D7_Pin | add) << 0x10;
#endif
}


/** @brief устанавливает 4/8 GPIO вывода в значения присланного байта/полубайта
 *  @note
 *  	В зависимости от выбранной конфигурации
 *  	LCD_DATA_WIDTH = LCD_DATA_WIDTH_BYTE / LCD_DATA_WIDTH_HALF_BYTE
 *  	Устанавливает 4 пина в значения бит младшего квартета
 *  	или 8 пинов в битовые значения всего бита
 *  	В зависимости от того, выбрана 4 или 8 бит конфигурация
 *  	Пины
 *  		D0_Pin -- 8 битная передача
 *  		D1_Pin -- 8 битная передача
 *  		D2_Pin -- 8 битная передача
 *  		D3_Pin -- 8 битная передача
 *  		D4_Pin -- 8/4 битная передача
 *  		D5_Pin -- 8/4 битная передача
 *  		D6_Pin -- 8/4 битная передача
 *  		D7_Pin -- 8/4 битная передача
 *  	Должны быть уже определены и должны принадлежать одному порту
 *  	Пины
 *  		RS_Pin -- строб данных
 *  		E_Pin  -- общий строб передачи
 *  	так же уже должны быть определены и принадлежать тому же порту
 *  	Для 8 битного режима передачи надо 10 пинов
 *  	Для 4 битного режима передачи надо 6 пинов
 *  @param [in] data передаваемый байт/полубайт
 *  @return None
 */
static void s_set_gpio (uint8_t data, uint32_t add)
{
#if	(LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE)
    // Установка необходимых бит данных
    GPIO_PORT->BSRR = ((data & 0x01) ? D4_Pin : D4_Pin << 0x10) |
                      ((data & 0x02) ? D5_Pin : D5_Pin << 0x10) |
                      ((data & 0x04) ? D6_Pin : D6_Pin << 0x10) |
                      ((data & 0x08) ? D7_Pin : D7_Pin << 0x10) |
					  ( add );
#elif (LCD_DATA_WIDTH == LCD_DATA_WIDTH_BYTE)
    // Установка необходимых бит данных
    GPIO_PORT->BSRR =
		((data & 0x01) ? D0_Pin : D0_Pin << 0x10) |
		((data & 0x02) ? D1_Pin : D1_Pin << 0x10) |
		((data & 0x04) ? D2_Pin : D2_Pin << 0x10) |
		((data & 0x08) ? D3_Pin : D3_Pin << 0x10) |
		((data & 0x10) ? D4_Pin : D4_Pin << 0x10) |
		((data & 0x20) ? D5_Pin : D5_Pin << 0x10) |
		((data & 0x40) ? D6_Pin : D6_Pin << 0x10) |
		((data & 0x80) ? D7_Pin : D7_Pin << 0x10) |
		 (add);
#endif
}

/** @brief Отправляет байт через пины
 *  @note
 *  	Стробирует E
 *  	Используется тупая задержка для отделения одного сигнала от другого
 *  	(Минимальное количество итераций 10 000)
 *  	Используется как для отправки полубайта, так и для отправки байта
 *  @return None
 */
static void s_transport_byte (uint8_t data, uint32_t add)
{
	s_set_gpio (data, add);         // Передача старшего полубайта
	s_stupid_delay(STUPID_DELAY);
	// s_reset_gpio (add);
	GPIO_PORT->BSRR |= (add << 0x10);
	s_stupid_delay(STUPID_DELAY);
}

/** @brief Отправляет байт, как данные (Взводится линия RS)
 *  @note
 *  	RS_Pin
 *  	E_Pin
 *  	Должны быть уже определены и принадлежать тому же порту,
 *  	что и пины данных
 *  	E_Pin стробирует передачу байта/полубайта
 *  @return None
 */
static void s_send_data (uint8_t data)
{
#if	(LCD_DATA_WIDTH == LCD_DATA_WIDTH_BYTE)
	s_transport_byte (data, E_Pin | RS_Pin);
#elif (LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE)
    s_transport_byte(data >> 4, E_Pin | RS_Pin);
    s_transport_byte(data, E_Pin | RS_Pin);
#endif
}

/** @brief Отправляет байт, как команду (Линия RS не стробируется)
 *  @note
 *  	RS_Pin -- не стробируется
 *  	E_Pin  -- стробируется
 *  	В этой реализации пины должны быть уже определены
 *  	и принадлежать тому же порту, что и пины данных
 *  @return None
 */
static void s_send_command (uint8_t data)
{
#if	(LCD_DATA_WIDTH == LCD_DATA_WIDTH_BYTE)
	s_transport_byte (data, E_Pin);
#elif (LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE)
    s_transport_byte ((data >> 4) & 0x0F, E_Pin);
    s_transport_byte (data & 0x0F, E_Pin);
#endif
    // Здесь нужна задержка больше 1.2 мс. Иначе инициализация проходит через раз
    HAL_Delay(1);
}

/** @brief Предварительный сброс управляющих пинов RS, RW, E и пинов даннных D0-D7
 *	@return None
 */
static void s_transport_init (void)
{
	s_reset_gpio(E_Pin | RS_Pin | RW_Pin);
}

#elif (LCD_DATA_TRANSPORT == LCD_DATA_74HC595)

static void s_send_8bit      (uint8_t data, uint8_t add); ///?> Отправляет 8 бит вне зависимости от режима отправки со стробами E и add
static void s_send_2x4bit    (uint8_t data, uint8_t add); ///?> Разбивает байт на два квартета и отправляет со стробами add
static void s_set_srclk      (void); ///?> Строб 74HC595 SRCLCK (пин 10)
static void s_transport_byte (uint8_t data);   ///?> Отправка байта с данными, зависящая от транспорта

/// Биты вывода 74HC595
#define BKL_Bit 0  ///?> Бит включения/выключения освещения подложки
#define RS_Bit  1  ///?> Бит RS (режим данных)
#define RW_Bit  2  ///?> Бит RW (записи в память)
#define E_Bit   3  ///?> Бит E строба данных/команды
#define D0_Bit  8  ///?> Бит 0 (D0, 8 битный режим)
#define D1_Bit  9  ///?> Бит 1 (D1, 8 битный режим)
#define D2_Bit  10 ///?> Бит 2 (D2, 8 битный режим)
#define D3_Bit  11 ///?> Бит 3 (D3, 8 битный режим)
#define D4_Bit  4  ///?> Бит 4 (D4, 8/4 битный режим)
#define D5_Bit  5  ///?> Бит 5 (D5, 8/4 битный режим)
#define D6_Bit  6  ///?> Бит 6 (D6, 8/4 битный режим)
#define D7_Bit  7  ///?> Бит 7 (D7, 8/4 битный режим)

/// Битовые маски выводов 74HC595
#define BKL_MSK (1 << BKL_Bit) ///?> Маска бита включения/выключения освещения подложки
#define RS_MSK  (1 << RS_Bit)  ///?> Маска бита RS (режим данных)
#define RW_MSK  (1 << RW_Bit)  ///?> Маска бита RS (режим данных)
#define EN_MSK  (1 << EN_Bit)  ///?> Бит E строба данных/команды
#define D0_MSK  (1 << D0_Bit)  ///?> Маска бита 0 (D0) 8 битный режим
#define D1_MSK  (1 << D1_Bit)  ///?> Маска бита 1 (D1) 8 битный режим
#define D2_MSK  (1 << D2_Bit)  ///?> Маска бита 2 (D2) 8 битный режим
#define D3_MSK  (1 << D3_Bit)  ///?> Маска бита 3 (D3) 8 битный режим
#define D4_MSK  (1 << D4_Bit)  ///?> Маска бита 4 (D4) 8/4 битный режим
#define D5_MSK  (1 << D3_Bit)  ///?> Маска бита 5 (D5) 8/4 битный режим
#define D6_MSK  (1 << D2_Bit)  ///?> Маска бита 6 (D6) 8/4 битный режим
#define D7_MSK  (1 << D1_Bit)  ///?> Маска бита 7 (D7) 8/4 битный режим

/** @brief Инициализация (если нужна) транспортного протокола.
 *  @note
 *  	Общая для всех видов транспорта
 *  @return None
 */
static void s_transport_init (void)
{
	// s_send_byte(0);
	SRCLK_GPIO_Port->BSRR = (SRCLK_Pin << 0x10); // Сбросить строб RSCLK (11) 74HC595
	RCLK_GPIO_Port->BSRR = (RCLK_Pin << 0x10); // Сбросить строб LATCH (12) 74HC595
	SER_GPIO_Port->BSRR = (SER_Pin << 0x10);   // Сбросить строб DATA  (14) 74HC595
}

/** @brief Отправляет байт, как команду (Линия RS не стробируется)
 *  @note
 *  	Общая для всех видов траспорта.
 *  	(Вызывается общей функцией LCD_SendData)
 *  		RS_Bit -- не стробируется
 *  		E_Bit  -- стробирует передачу байта/полубайта
 *  	Для 4-битного режима: старший квартет -- данные, младший -- управляющие биты (E, RS, RW)
 *  	Прокси/заглушка для выбора между 8/4 бит и отправки команды без стробирования RS
 *  @param data (uint8_t) данные для отправки
 *  @return None
 */
static void s_send_command (uint8_t data)
{
#if	(LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE)
	s_send_2x4bit(data, 0); // Отправить команду, разбив на два квартета в старшем полубайте
#elif (LCD_DATA_WITH == LCD_DATA_WIDTH_BYTE)
#error "8 битный режим передачи данных для 74HC595 не реализован"
#endif
}

/** @brief Отправляет байт, как данные (Линия RS стробируется)
 *  @note
 *  	Должна быть определена во всех видах транспорта
 *  	(Вызывается общей функцией LCD_SendCommand)
 *  	RS_Bit -- стробируется
 *  	E_Bit  -- стробирует передачу байта/полубайта
 *  	В 4-битном режиме старший квартет -- данные, младший -- управляющие биты (E, RS)
 *  @param data (uint8_t)
 *  @return None
 */
static void s_send_data (uint8_t data)
{
#if	(LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE)
	s_send_2x4bit(data, RS_MSK); // Отправить данные разбив два квартета (в старшем полубайте)
#elif (LCD_DATA_WITH == LCD_DATA_WIDTH_BYTE)
#error "8 битный режим передачи данных для 74HC595 не реализован"
#endif
}

/** @brief Отправляет 8 бит со стробированием E и add (если != 0)
 *  @note
 *  	Отправляет 8 бит вне зависимости от режима передачи 8/4 бита данных
 *  	Стробит передачу при помощи E_Bit и если add != 0, содержимым add
 *  	add передавать уже со смещением (маска, MSK)
 *  @param
 *	@return None
 */
static void s_send_8bit (uint8_t data, uint8_t add)
{
	s_transport_byte(data | BKL_MSK | EN_MSK | add);
	s_transport_byte(data | BKL_MSK);
}

/** @brief Отправляет байт в 4-битном режиме передачи данных
 *  @note
 *  	В режиме данных 4 бита разбивает данные на 2 байта
 *  		в которых:
 *  			1. старший квартет -- данные,
 *  			2. младший квартет -- управляющие биты (LCD1602)
 *  	add может содержать
 *  		E_Bit  -- Строб передачи данных
 *  		RS_Bit -- Строб передачи команды
 *  		RW_Bit -- Строб записи в память (например, символа)
 *  	D0, D1, D2, D3, D4, D5, D6, D7 -- для 8битной передачи
 *  	D4, D5, D6, D7                 -- для 4битной передачи
 *  @return None
 */
static void s_send_2x4bit (uint8_t data, uint8_t add)
{
	// Отправить содержимое старшего квартета данных
	// и установить добавочные данные в младший полубит
    s_send_8bit((data) & 0xF0, add);
    // Отправить содержимое младшего квартета данных в старшем квартете
    // и установить добавочные данные в младший полубит
    s_send_8bit((data << 4) & 0xF0, add);
}

/** @brief Устанавливает сигнал на пине 74HC595 SRCLK (11)
 *  @note
 *		Функция реализована для транспорта 74HC595
 *	@return None
 */
static void s_set_srclk(void)
{
	s_stupid_delay(50); // Подождать 50 циклов
	SRCLK_GPIO_Port->BSRR = (SRCLK_Pin); // Установить пин SRCLK
	s_stupid_delay(50); // Подождать 50 циклов
	SRCLK_GPIO_Port->BSRR = (SRCLK_Pin << 0x10); // Сбросить пин SRCLK
}

/** @brief отправка байта/полубайта через выбранный транспорт
 *  @note
 *  	Должна быть реализована во всех реализация транспорта
 *  	Транспорты:
 *  		1. Пины 4/8-бит + Управляющие пины
 *  		2. 74HC595 данные 4-бит
 *  		3. PCF8574T данные 4-бит
 *  	Режим работы:
 *  		1. Запуск клок-сигнала для передачи битов (c 1)
 *  		2. Передача по дата-каналу 0/1
 *  		3. После завершения байта взвести защёлку (LATCH)
 *  	Пины подключены в последовательности
 *  		QD -> D4
 *  		QE -> D5
 *  		QF -> D6
 *  		QH -> D7
 *  	Отправка идёт с 7 по 0 бит
 */
static void s_transport_byte (uint8_t data)
{
	RCLK_GPIO_Port->BSRR = RCLK_Pin << 0x10;
	uint8_t cnt = 8; // Счётчик бит
	s_set_srclk ();
	while(cnt --) // Счёт от 7 до 0
	{
		SER_GPIO_Port->BSRR  = ((data >> cnt) & 1) ? (SER_Pin) : (SER_Pin << 0x10);
		s_set_srclk ();
	}
	RCLK_GPIO_Port->BSRR = (RCLK_Pin); // Установить защёлку и открыть установленные данные на передачу на пинах QA-QH 74HC595
	SER_GPIO_Port->BSRR  = (SER_Pin << 0x10); // Сбросить пин данных в 0
	s_stupid_delay(50);
}
#elif LCD_DATA_TRANSPORT == LCD_DATA_PCF8574T

#include "i2c.h"

#define PCF8574T_I2C_ADDR 0x27
#define RS_Bit  0 ///?> Если на пине RS -- 0, данные воспринимаются как команда, если 1 - как символы для вывода
#define RW_Bit  1 ///?> Пин записи данных в память LCD1602
#define EN_Bit  2 ///?> Строб данных на пинах LCD1602
#define BKL_Bit 3 ///?> Пин подсветки


#define PCF8574T_I2C_ADDR_MSK (PCF8574T_I2C_ADDR << 1) ///?> Адрес I2C, сдвинутый на 1 бит влево (HAL работает с I2C-адресами, сдвинутыми на 1 бит влево)
#define RS_MSK    (1 << RS_Bit)  ///?> Маска строба данных/команды
#define RW_MSK    (1 << RW_Bit)  ///?> Строб записи данных в память LCD1602
#define EN_MSK    (1 << EN_Bit)  ///?> Строб передачи данных на GPIO дисплея LCD1602
#define BKL_MSK   (1 << BKL_Bit) ///?> Управление подсветкой (BackLight)

#define HI2C_DEVICE_HANDLER hi2c1 ///?> идентификатор I2C

static void s_transport_byte (uint8_t data);
static void s_send_8bit      (uint8_t data, uint8_t add);
static void s_send_2x4bit    (uint8_t data, uint8_t add);

/** @brief Отправляет байт, как данные (Линия RS стробируется)
 *  @note
 *  	Должна быть определена во всех видах транспорта
 *  	(Вызывается общей функцией LCD_SendCommand)
 *  	RS_Bit -- стробируется
 *  	E_Bit  -- стробирует передачу байта/полубайта
 *  	В 4-битном режиме старший квартет -- данные, младший -- управляющие биты (E, RS)
 *  @param data (uint8_t)
 *  @return None
 */
static void s_send_data (uint8_t data)
{
#if	(LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE)
	s_send_2x4bit(data, RS_MSK); // Отправить данные разбив два квартета (в старшем полубайте)
#elif (LCD_DATA_WITH == LCD_DATA_WIDTH_BYTE)
#error "8 битный режим передачи данных для 74HC595 не реализован"
#endif
}

/** @brief Отправляет байт, как команду (Линия RS не стробируется)
 *  @note
 *  	Общая для всех видов траспорта.
 *  	(Вызывается общей функцией LCD_SendData)
 *  		RS_Bit -- не стробируется
 *  		EN_Bit  -- стробирует передачу байта/полубайта
 *  	Для 4-битного режима: старший квартет -- данные, младший -- управляющие биты (E, RS, RW)
 *  	Прокси/заглушка для выбора между 8/4 бит и отправки команды без стробирования RS
 *  @param data (uint8_t) данные для отправки
 *  @return None
 */
static void s_send_command (uint8_t data)
{
#if	(LCD_DATA_WIDTH == LCD_DATA_WIDTH_HALF_BYTE)
	s_send_2x4bit(data, 0); // Отправить команду, разбив на два квартета в старшем полубайте
#elif (LCD_DATA_WITH == LCD_DATA_WIDTH_BYTE)
#error "8 битный режим передачи данных для 74HC595 не реализован"
#endif
}

static void s_transport_init (void)
{

}

/** @brief Отправляет 8 бит со стробированием E и add (если != 0)
 *  @note
 *  	Отправляет 8 бит вне зависимости от режима передачи 8/4 бита данных
 *  	Стробит передачу при помощи E_Bit и если add != 0, содержимым add
 *  	add передавать уже со смещением (маска, MSK)
 *  @param
 *	@return None
 */
static void s_send_8bit (uint8_t data, uint8_t add)
{
	s_transport_byte(data | BKL_MSK | EN_MSK | add);
	s_transport_byte(data | BKL_MSK);
}

/** @brief Отправляет байт в 4-битном режиме передачи данных
 *  @note
 *  	В режиме данных 4 бита разбивает данные на 2 байта
 *  		в которых:
 *  			1. старший квартет -- данные,
 *  			2. младший квартет -- управляющие биты (LCD1602)
 *  	add может содержать
 *  		E_Bit  -- Строб передачи данных
 *  		RS_Bit -- Строб передачи команды
 *  		RW_Bit -- Строб записи в память (например, символа)
 *  	D0, D1, D2, D3, D4, D5, D6, D7 -- для 8битной передачи
 *  	D4, D5, D6, D7                 -- для 4битной передачи
 *  @return None
 */
static void s_send_2x4bit (uint8_t data, uint8_t add)
{
	// Отправить содержимое старшего квартета данных
	// и установить добавочные данные в младший полубит
    s_send_8bit((data) & 0xF0, add);
    // Отправить содержимое младшего квартета данных в старшем квартете
    // и установить добавочные данные в младший полубит
    s_send_8bit((data << 4) & 0xF0, add);
}

/** @brief Отправка байта
 *	@note
 *	@param [in] data -- байт для передачи
 *	@return None
 */
static void s_transport_byte (uint8_t data)
{
	// Проверка готовности устройства для отправки данных
	if (HAL_I2C_IsDeviceReady(& HI2C_DEVICE_HANDLER, PCF8574T_I2C_ADDR_MSK, 10, 1000) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_I2C_Master_Transmit(& HI2C_DEVICE_HANDLER, PCF8574T_I2C_ADDR_MSK, &data, 1, 1000);
}
#endif
