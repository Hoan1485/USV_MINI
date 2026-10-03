/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ATGM336H/atgm336.h"
#include "DS18B20/ds18b20.h"
#include "INA219/ina219.h"
#include "MPU6050/mpu6050.h"
#include "QM5883P.h"
#include "esp32_comm.h"
#include "stepper.h"
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
ATGM336H_Data_t gps_data;
uint8_t gps_rx_data;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void MPU6050_SendToUART(MPU6050_Data_t *data);
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("Starting I2C Sensors...\r\n");

  if (MPU6050_Init() == HAL_OK) {
    printf("MPU6050 Init SUCCESS\r\n");
  } else {
    printf("MPU6050 Init FAILED\r\n");
  }

  if (QMC5883P_Init() == HAL_OK) {
    printf("QMC5883P Init SUCCESS\r\n");
  } else {
    printf("QMC5883P Init FAILED\r\n");
  }

  if (INA219_Init(&hi2c1) == HAL_OK) {
    printf("INA219 Init SUCCESS\r\n");
  } else {
    printf("INA219 Init FAILED\r\n");
  }

  DS18B20_Init();
  DS18B20_Request_Temp();

  extern void MX_Stepper_GPIO_Init(void);
  MX_Stepper_GPIO_Init();

  ATGM336H_Init(&gps_data);
  HAL_UART_Receive_IT(&huart1, &gps_rx_data, 1);
  ESP32Comm_Init(&huart2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t last_status_tick = 0;
  uint32_t last_temp_req = 0;
  uint32_t last_ina_req = 0;

  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    MPU6050_Data_t imu = {0};
    QMC5883P_Data_t mag = {0};

    MPU6050_ReadRaw(&imu);
    QMC5883P_ReadRaw(&mag);

    float heading = atan2f(mag.My, mag.Mx) * 180.0f / 3.14159265f;
    if (heading < 0.0f) {
      heading += 360.0f;
    }
    g_usv_state.heading = heading;

    if (HAL_GetTick() - last_temp_req >= 750) {
      last_temp_req = HAL_GetTick();
      g_usv_state.water_temp = DS18B20_Read_Temp();
      DS18B20_Request_Temp();
    }

    if (HAL_GetTick() - last_ina_req >= 200) {
      last_ina_req = HAL_GetTick();
      INA219_ReadBusVoltage_V(&hi2c1, &g_usv_state.battery_volt);
      INA219_ReadCurrent_A(&hi2c1, &g_usv_state.current_amp);
    }

    g_usv_state.current_lat = gps_data.latitude;
    g_usv_state.current_lon = gps_data.longitude;
    g_usv_state.speed = gps_data.speed_kph / 3.6f;
    g_usv_state.gps_valid = (gps_data.valid != 0);

    ESP32Comm_Process();

    if (HAL_GetTick() - last_status_tick >= 500) {
      last_status_tick = HAL_GetTick();
      ESP32Comm_SendStatus();
    }

    stepper_update();
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
// Hàm in lỗi ra Data Console ở STM32
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE {
  ITM_SendChar(ch);
  return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    ATGM336H_ProcessChar((char)gps_rx_data, &gps_data);
    HAL_UART_Receive_IT(&huart1, &gps_rx_data, 1);
  } else if (huart->Instance == USART2) {
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
