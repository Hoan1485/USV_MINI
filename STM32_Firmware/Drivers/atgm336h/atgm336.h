#ifndef __ATGM336H_H
#define __ATGM336H_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Structure to hold the parsed GPS data */
typedef struct {
    float latitude;      // Latitude in decimal degrees
    float longitude;     // Longitude in decimal degrees
    float speed_knots;   // Speed in knots
    float speed_kph;     // Speed in kilometers per hour
    float course;        // Course over ground in degrees
    float altitude;      // Altitude in meters
    uint8_t fix_quality; // 0 = Invalid, 1 = GPS fix, 2 = DGPS fix
    uint8_t satellites;  // Number of satellites in view
    uint8_t valid;       // 1 = Data valid, 0 = Data invalid
} ATGM336H_Data_t;

/* Initialize the ATGM336H data structure */
void ATGM336H_Init(ATGM336H_Data_t *gps_data);

/* Process a single character received from UART. 
 * Call this function inside HAL_UART_RxCpltCallback or your polling loop.
 * Returns 1 if a complete valid sentence was successfully parsed, 0 otherwise.
 */
uint8_t ATGM336H_ProcessChar(char c, ATGM336H_Data_t *gps_data);

#endif /* __ATGM336H_H */
