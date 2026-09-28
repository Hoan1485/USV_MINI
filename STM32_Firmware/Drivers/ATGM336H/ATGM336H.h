#ifndef __ATGM336H_H
#define __ATGM336H_H

#include "main.h"
#include <stdint.h>

/* Structure to hold the parsed GPS data */

typedef struct {
	float latitude;			// Vĩ độ
	float longitude;		// Kinh độ
	float speed_knots;		// Tốc độ (Knots)
	float speed_kph;		// Tốc độ (Km/h)
	float course;			// Hướng di chuyển
	float altitude;			// Độ cao so với mực nước biển
	uint8_t fix_quality;
	uint8_t satellite;
	uint8_t valid;
} ATGM336H_Data_t;

/* Khởi tạo bộ lưu trữ dữ liệu GPS */
void init_gps(ATGM336H_Data_t *gps_data);

/* Hàm xử lý từng chữ cái nhận được từ mạch GPS qua UART.
 * Gọi hàm này bên trong ngắt nhận UART.
 * Hàm sẽ trả về 1 nếu nhận đủ và đọc thành công 1 câu dữ liệu, nếu không trả về 0.
 */
uint8_t process_gps_char(char c, ATGM336H_Data_t *gps_data);

#endif

