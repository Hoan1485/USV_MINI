#include "main.h"
#include "ds18b20.h"
#include "gpio.h"
#include "i2c.h"
#include "qmc5883l.h"
#include "tim.h"
#include "usart.h"
#include <mpu6050.h>

#include <stdio.h>
#include <math.h>
#include "atgm336.h"
#include "esp32_comm.h"

void SystemClock_Config(void);

void MPU6050_SendToUART();

ATGM336H_Data_t gps_data;
uint8_t gps_rx_data;

int main(void) {
  HAL_Init();

  /* Configure the system clock first */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();

  /* MPU6050 và QMC5883L init must be after I2C init and SystemClock_Config */
  MPU6050_Init();
  QMC5883L_Init();
  DS18B20_Init();
  DS18B20_Request_Temp(); // Bắt đầu quá trình đo nhiệt độ đầu tiên

  /* USER CODE BEGIN 2 */
  ATGM336H_Init(&gps_data);
  HAL_UART_Receive_IT(&huart1, &gps_rx_data, 1);
  ESP32Comm_Init(&huart3);
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t last_status_tick = 0;
  uint32_t last_temp_req = 0;

  while (1) {
    MPU6050_Data_t imu = {0};
    QMC5883L_Data_t mag = {0};
    /* USER CODE END WHILE */

    /* 1. Đọc dữ liệu cảm biến */
    MPU6050_ReadRaw(&imu);
    QMC5883L_ReadRaw(&mag);

    /* 2. Tính góc hướng la bàn (Heading) từ QMC5883L */
    float heading = atan2f(mag.My, mag.Mx) * 180.0f / 3.14159265f;
    if (heading < 0.0f) {
      heading += 360.0f;
    }
    g_usv_state.heading = heading;

    /* 3. Cập nhật nhiệt độ nước từ DS18B20 (mỗi 750ms) */
    if (HAL_GetTick() - last_temp_req >= 750) {
      last_temp_req = HAL_GetTick();
      g_usv_state.water_temp = DS18B20_Read_Temp();
      DS18B20_Request_Temp();
    }

    /* 4. Cập nhật dữ liệu GPS */
    g_usv_state.current_lat = gps_data.latitude;
    g_usv_state.current_lon = gps_data.longitude;
    g_usv_state.speed = gps_data.speed_kph / 3.6f; /* m/s */
    g_usv_state.gps_valid = (gps_data.valid != 0);

    /* 5. Xử lý lệnh nhận được từ ESP32 */
    ESP32Comm_Process();

    /* 6. Định kỳ 500ms gửi STATUS lên ESP32 Web Dashboard */
    if (HAL_GetTick() - last_status_tick >= 500) {
      last_status_tick = HAL_GetTick();
      ESP32Comm_SendStatus();
    }

    HAL_Delay(10);

    /* USER CODE BEGIN 3 */
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
void MPU6050_SendToUART(MPU6050_Data_t *data) {
  char txBuffer[150]; // Tạo một mảng bộ đệm chứa chuỗi ký tự

  // 1. In dữ liệu đã convert ra đơn vị thực tế (float)
  // Gia tốc: đơn vị g, Gyro: đơn vị deg/s
  int len = sprintf(txBuffer,
                    "ACC (g): X=%.2f, Y=%.2f, Z=%.2f | GYRO (deg/s): X=%.2f, "
                    "Y=%.2f, Z=%.2f\r\n",
                    data->Ax, data->Ay, data->Az, data->Gx, data->Gy, data->Gz);

  // 2. Lệnh của STM32 để đẩy toàn bộ chuỗi ký tự này ra cổng UART3
  // Thời gian chờ tối đa (timeout) là 100ms
  HAL_UART_Transmit(&huart3, (uint8_t *)txBuffer, len, 100);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    ATGM336H_ProcessChar((char)gps_rx_data, &gps_data);
    HAL_UART_Receive_IT(&huart1, &gps_rx_data, 1);
  } else if (huart->Instance == USART3) {
    ESP32Comm_RxCpltCallback(huart);
  }
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
