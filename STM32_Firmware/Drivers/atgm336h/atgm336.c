#include "atgm336.h"
#include <string.h>
#include <stdlib.h>
#include "../../Application/Inc/esp32_comm.h"

#define ATGM336H_BUFFER_SIZE 128

static char rx_buffer[ATGM336H_BUFFER_SIZE];
static uint16_t rx_index = 0;

static uint32_t last_err_tick = 0;

void ATGM336H_Init(ATGM336H_Data_t *gps_data) {
    memset(gps_data, 0, sizeof(ATGM336H_Data_t));
    rx_index = 0;
}

// Helper to convert NMEA coordinate (ddmm.mmmm) to decimal degrees
static float NMEA_To_Decimal(float nmea_coord, char direction) {
    int degrees = (int)(nmea_coord / 100);
    float minutes = nmea_coord - (degrees * 100);
    float decimal = degrees + (minutes / 60.0f);
    
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    return decimal;
}

// Kiểm tra mã Checksum của chuỗi NMEA
static uint8_t ATGM336H_VerifyChecksum(const char *sentence) {
    if (sentence[0] != '$') return 0;
    uint8_t checksum = 0;
    int i = 1;
    while (sentence[i] != '*' && sentence[i] != '\0') {
        checksum ^= sentence[i];
        i++;
    }
    if (sentence[i] == '*') {
        // Chuyển 2 ký tự Hex sau dấu * thành số
        int received_checksum = (int)strtol(&sentence[i + 1], NULL, 16);
        return (checksum == received_checksum);
    }
    return 0;
}

// Parses a complete NMEA sentence
static uint8_t ATGM336H_ParseSentence(char *sentence, ATGM336H_Data_t *gps_data) {
    char *p = sentence;
    char *fields[20];
    int field_count = 0;
    
    // Split the sentence by commas, preserving empty fields
    fields[field_count++] = p;
    while (*p && field_count < 20) {
        if (*p == ',') {
            *p = '\0';
            fields[field_count++] = p + 1;
        }
        p++;
    }
    
    // Ensure we have a command field
    if (field_count == 0) return 0;
    
    // Check if it's an RMC sentence (Recommended Minimum Specific GNSS Data)
    if (strstr(fields[0], "RMC") != NULL) {
        if (field_count > 9) {
            // Field 2: Status (A = Active, V = Void)
            if (fields[2][0] == 'A') gps_data->valid = 1;
            else gps_data->valid = 0;
            
            // Field 3: Latitude, Field 4: N/S indicator
            if (strlen(fields[3]) > 0 && strlen(fields[4]) > 0) {
                gps_data->latitude = NMEA_To_Decimal(atof(fields[3]), fields[4][0]);
            }
            
            // Field 5: Longitude, Field 6: E/W indicator
            if (strlen(fields[5]) > 0 && strlen(fields[6]) > 0) {
                gps_data->longitude = NMEA_To_Decimal(atof(fields[5]), fields[6][0]);
            }
            
            // Field 7: Speed over ground in knots
            if (strlen(fields[7]) > 0) {
                gps_data->speed_knots = atof(fields[7]);
                gps_data->speed_kph = gps_data->speed_knots * 1.852f;
            }
            
            // Field 8: Course over ground in degrees
            if (strlen(fields[8]) > 0) {
                gps_data->course = atof(fields[8]);
            }
            return 1;
        }
    }
    // Check if it's a GGA sentence (Global Positioning System Fix Data)
    else if (strstr(fields[0], "GGA") != NULL) {
        if (field_count > 9) {
            // Field 6: Fix quality
            if (strlen(fields[6]) > 0) {
                gps_data->fix_quality = atoi(fields[6]);
            }
            
            // Field 7: Number of satellites
            if (strlen(fields[7]) > 0) {
                gps_data->satellites = atoi(fields[7]);
            }
            
            // Field 9: Altitude
            if (strlen(fields[9]) > 0) {
                gps_data->altitude = atof(fields[9]);
            }
            return 1;
        }
    }
    
    return 0; // Not a sentence we care about or not enough fields
}

uint8_t ATGM336H_ProcessChar(char c, ATGM336H_Data_t *gps_data) {
    if (c == '$') {
        rx_index = 0;
        rx_buffer[rx_index++] = c;
    } else if (rx_index > 0 && rx_index < ATGM336H_BUFFER_SIZE - 1) {
        rx_buffer[rx_index++] = c;
        if (c == '\n') { // End of sentence
            rx_buffer[rx_index] = '\0'; // Null-terminate
            
            if (!ATGM336H_VerifyChecksum(rx_buffer)) {
                if (HAL_GetTick() - last_err_tick > 2000) {
                    last_err_tick = HAL_GetTick();
                    ESP32Comm_SendError("GPS_CHECKSUM_ERR");
                }
                rx_index = 0;
                return 0;
            }

            // Temporary buffer because parsing modifies the string
            char temp_buffer[ATGM336H_BUFFER_SIZE];
            strcpy(temp_buffer, rx_buffer);
            
            uint8_t parsed = ATGM336H_ParseSentence(temp_buffer, gps_data);
            rx_index = 0;
            return parsed;
        }
    } else if (rx_index >= ATGM336H_BUFFER_SIZE - 1) {
        // Buffer overflow, reset
        if (HAL_GetTick() - last_err_tick > 2000) {
            last_err_tick = HAL_GetTick();
            ESP32Comm_SendError("GPS_OVERFLOW");
        }
        rx_index = 0;
    }
    return 0;
}
