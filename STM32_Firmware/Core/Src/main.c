#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "qmc5883l.h"
#include "tim.h"
#include "usart.h"
#include <mpu6050.h>


#include <stdio.h>

void SystemClock_Config(void);

void MPU6050_SendToUART();

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

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    MPU6050_Data_t imu;
    QMC5883L_Data_t mag;
    /* USER CODE END WHILE */
    MPU6050_ReadRaw(&imu);
    QMC5883L_ReadRaw(&mag);

    char txBuffer[150];
    int len = sprintf(
        txBuffer,
        "ACC: %.2f %.2f %.2f | GYR: %.2f %.2f %.2f | MAG: %.2f %.2f %.2f\r\n",
        imu.Ax, imu.Ay, imu.Az, imu.Gx, imu.Gy, imu.Gz, mag.Mx, mag.My, mag.Mz);
    HAL_UART_Transmit(&huart3, (uint8_t *)txBuffer, len, 100);

    HAL_Delay(200);

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
