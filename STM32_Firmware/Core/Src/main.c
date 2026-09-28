/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include <ATGM336H/ATGM336H.h>
#include <DS18B20/DS18B20.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "comm.h"
#include "gpio.h"
#include "i2c.h"
#include "motor.h"
#include "nav.h"
#include "stepper.h"
#include "tim.h"
#include "usart.h"
#include <MPU6050/MPU6050.h>
#include <QM5883P/QM5883P.h> // Tên file vẫn giữ nguyên, nhưng code bên trong là QMC5883P
#include <math.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// Biến lưu trữ dữ liệu GPS (tọa độ, tốc độ...)
ATGM336H_Data_t gps_data;
// Biến nhận từng ký tự từ GPS
uint8_t gps_rx_data;

// Bộ điều khiển PID cho việc giữ hướng (Heading)
// Các thông số Kp = 5.0, Ki = 0.0, Kd = 1.0 (Có thể cần tinh chỉnh thêm khi
// chạy thực tế)
PID_Controller heading_pid = {5.0f, 0.0f, 1.0f, 0.0f, 0.0f};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void SystemClock_Config(void);
void Control_Motor(int left_speed, int right_speed);
void Debug_TxData(float lat, float lon, float heading, float temp, float speed,
                  MPU6050_Data_t *imu);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  SystemCoreClockUpdate(); // Cập nhật lại biến lưu tần số chip (quan trọng cho
                           // DS18B20)

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();

  /* USER CODE BEGIN 2 */
  init_mpu6050();
  QMC5883P_Init(); // Bạn đã lỡ tay xóa mất dòng này ở bản trước nên nó mới ra 0
                   // độ!
  init_ds18b20();
  request_temperature();

  //  init_gps(&gps_data);
  //  HAL_UART_Receive_IT(&huart1, &gps_rx_data, 1);
  //  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  //  Comm_Init(); // Kích hoạt USART2 nhận lệnh từ ESP32
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t last_telemetry_time = 0;

  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    // 1. Cập nhật động cơ bước liên tục (chạy ngầm, không gây treo máy)
    //    stepper_update();

    // 2. Kích hoạt rải mồi nếu có lệnh từ ESP32
    //    if (feed_request_flag == 1) {
    //      drop_bait(); // Chỉ đặt mục tiêu quay, stepper_update() sẽ tự làm
    //      nốt feed_request_flag = 0;
    //    }

    // 3. --- ĐIỀU KHIỂN ĐỘNG CƠ CỦA TÀU ---
    // Kiểm tra tính năng an toàn (mất sóng quá 3s tự dừng động cơ)
    //    Comm_CheckFailsafe();

    // Gửi lệnh xuống ESC/Motor
    //    if (current_mode == MODE_MANUAL) {
    //      Control_Motor(manual_left_speed, manual_right_speed);
    //    } else if (current_mode == MODE_AUTO) {
    //      // (Dành cho sau này khi bạn viết xong tính năng tự lái Nav)
    //    }
    // ---------------------------

    // 4. Đọc cảm biến và gửi Telemetry mỗi 100ms (10 lần/s)
    if (HAL_GetTick() - last_telemetry_time >= 100) {
      last_telemetry_time = HAL_GetTick();

      MPU6050_Data_t imu = {0}; // Giữ lại khai báo để truyền vào hàm Debug
      QMC5883P_Data_t mag = {0};

      // Đã ẩn các cảm biến khác
      // read_mpu6050_data(&imu);
      QMC5883P_ReadRaw(&mag);
      float ds18b20_temp = 0.0f; // Giá trị ảo để không bị lỗi undefined
      // request_temperature();

      // Tính góc La bàn (Heading) có kèm bộ lọc nhiễu (Low-Pass Filter)
      static float filtered_Mx = 0.0f;
      static float filtered_My = 0.0f;

      if (filtered_Mx == 0.0f && filtered_My == 0.0f) {
        filtered_Mx = mag.Mx; // Lấy mẫu đầu tiên
        filtered_My = mag.My;
      } else {
        // Hệ số 0.1: Càng nhỏ kim càng mượt nhưng trễ, lớn thì nhanh nhưng giật
        filtered_Mx = (filtered_Mx * 0.9f) + (mag.Mx * 0.1f);
        filtered_My = (filtered_My * 0.9f) + (mag.My * 0.1f);
      }

      float heading = atan2f(filtered_My, filtered_Mx) * 180.0f / 3.14159265f;
      if (heading < 0)
        heading += 360.0f;

      // Debug qua cổng PC (USART3)
      Debug_TxData(gps_data.latitude, gps_data.longitude, heading, ds18b20_temp,
                   gps_data.speed_kph, &imu);

      // Gửi Telemetry cho ESP32 (USART2)
      // Comm_SendTelemetry(gps_data.latitude, gps_data.longitude, heading,
      //                    ds18b20_temp, gps_data.speed_kph);
      HAL_Delay(500);
    }
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// Hàm gửi TẤT CẢ dữ liệu qua USART3 (Dùng để kiểm tra lỗi - Debug trên máy
// tính)
void Debug_TxData(float lat, float lon, float heading, float temp, float speed,
                  MPU6050_Data_t *imu) {
  char txBuffer[256];

  // Gộp tất cả thông tin thành một đoạn văn bản dễ đọc
  int len = sprintf(txBuffer,
                    "\r\n--- THONG SO TAU ---\r\n"
                    "GPS: Vi do = %.6f, Kinh do = %.6f, Toc do= %.1f km/h\r\n"
                    "La ban (Heading): %.1f do\r\n"
                    "Nhiet do nuoc: %.1f C\r\n"
                    "MPU6050: Gx = %.1f, Gy = %.1f, Gz = %.1f\r\n"
                    "--------------------\r\n",
                    lat, lon, speed, heading, temp, imu->Gx, imu->Gy, imu->Gz);

  // Đẩy chuỗi ký tự này ra cổng UART3 để hiển thị lên màn hình máy tính
  HAL_UART_Transmit(&huart3, (uint8_t *)txBuffer, len, 100);
}

// Hàm điều khiển tốc độ động cơ viết sẵn để bạn dễ dàng gọi ở bất kỳ đâu trong
// file main.c
void Control_Motor(int left_speed, int right_speed) {
  // Lệnh này gọi xuống hàm ở file motor.c
  motor_set_speed(left_speed, right_speed);
}

// Hàm ngắt chung của UART, được gọi mỗi khi có dữ liệu đến ở BẤT KỲ cổng UART
// nào
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  // Nếu dữ liệu đến từ USART1 (Nối với mạch GPS)
  if (huart->Instance == USART1) {
    // Đưa chữ cái vừa nhận vào hàm xử lý GPS
    process_gps_char((char)gps_rx_data, &gps_data);
    // Kích hoạt lại ngắt để nhận chữ cái GPS tiếp theo
    HAL_UART_Receive_IT(&huart1, &gps_rx_data, 1);
  }

  // Gọi hàm ngắt của file giao tiếp (để xử lý dữ liệu từ ESP32 trên UART3)
  extern void Comm_UART_RxCpltCallback(UART_HandleTypeDef * huart);
  Comm_UART_RxCpltCallback(huart);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
