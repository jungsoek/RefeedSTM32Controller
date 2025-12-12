/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* CDC set  up */
//#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// Receive buffer
RTC_AlarmTypeDef sAlarm = {0};
RTC_TimeTypeDef sTime = {0};

uint8_t LOG_buffer[UART_RX_BUFFER_SIZE];
// Sending buffer
char log_msg[UART_RX_BUFFER_SIZE];
volatile bool LOG_DataValid = false;

//Wake up
volatile bool EXTI_Wakeup = false;
volatile bool Alarm_Wakeup = false;
volatile bool flag_EXTI = false;
//POST PhoneNum
char PhoneNum[20];

//Data SIM, id
const char ID[] = "testmachine";
const char countryDialCode[] = "+84";
const char url_postper[] = "https://api.admin.bi-oil.app/postcontainer";
const char url_identity[] = "https://api.admin.bi-oil.app/identity";
const char url_postuco[] = "https://api.admin.bi-oil.app/postuco";
static char data_PostSimCom[150];

//Data herder
char data_TransmitHeader[100];


//Value water and oil
float volume_water; //(Liter)
float volume_oil;   //(Liter)

//Volume on container and battery
float value_VolContainer; // (%)
float value_Voltage;	  // (V)

float distance_Tof = -1.0f;
float value_LoadCell = -1.0f;
int water_weight = -1;
int watersensor_value = -1;
float volume_total = -1.0f;
//float volume_WS = -1.0f;
/////
char data_Ok[] = "OK";
char data_Fail[] = "FAIL";
char data_Oil_Water[] = "O1.23W4.56E";
const char* data_Post = "{\"volume\": 123,\"battery\": 456}";

typedef enum {
	HANDSHAKE,
	VALIDATION,
	POST_UCO,
	POST_PER,
	OPEN_INPUT,
	CLOSE_INPUT,
	UNLOCK_DOOR,
	SLEEP,
	ALARM_WAKEUP,
	EXTI_WAKEUP,
	OPEN_VALVE,
	CLOSE_VALVE,
	TURN_OFF,
	TEST_PERCENT,
	TEST_MEASURE,
	TEST_SDCLOSE,
	TEST_SDOPEN,
	SPACE
} Msg_Header;

typedef enum {
	ERR_SIMCOM,
	ERR_ULTRA,
	ERR_TOF,
	ERR_LOADCELL
} Err_Device;

Msg_Header FUNCTION;


//typedef enum {
//	IDLE,			// Run without doing anything and not sleep
//	INIT,			// Initiate peripheral and handshake
//	VALID,			// Check if user is valid or not QR and NUM input, Update number or QR, machine id to the server
//	OPENS,			// Open small door
//	POSTPER,		// Update the container percentage
//	CLOSES,			// Close the door and start the measuring process
//	POSTDATA,		// Update the user id, input amount and machine if to the server
//	SLEEP,			// Sleep allowance and go to sleep
//	ALARM,			// Display update and stop user from using machine
//} command;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void CheckHeader(void){
  //echo response for ack
  LOG("[ANS]","ACK");
	if(strstr((char *)LOG_buffer, "[CMD]HANDSHAKE")) {
		FUNCTION = HANDSHAKE;
	}
	else if(strstr((char *)LOG_buffer, "[VALID]") && strstr((char *)LOG_buffer, "ENDSTR")) {
		FUNCTION = VALIDATION;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]OPENS")) {
		FUNCTION = OPEN_INPUT;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]CLOSES")) {
		FUNCTION = CLOSE_INPUT;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]POSTPER")) {
		FUNCTION = POST_PER;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]POSTDATA")){
		FUNCTION = POST_UCO;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]OPENB")) {
		FUNCTION = UNLOCK_DOOR;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]SLEEP")) {
		FUNCTION = SLEEP;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]CLOSEV")) {
		FUNCTION = CLOSE_VALVE;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]OPENV")) {
		FUNCTION = OPEN_VALVE;
	}
	else if(strstr((char *)LOG_buffer, "[CMD]OFF")) {
		FUNCTION = TURN_OFF;
	}////////////////////////////////////////////////////////
	else if(strstr((char *)LOG_buffer, "[TEST]PERCENT")) {
		FUNCTION = TEST_PERCENT;
	}
	else if(strstr((char *)LOG_buffer, "[TEST]MEASURE")) {
		FUNCTION = TEST_MEASURE;
	}
	else if(strstr((char *)LOG_buffer, "[TEST]SDCLOSE")) {
		FUNCTION = TEST_SDCLOSE;
	}
	else if(strstr((char *)LOG_buffer, "[TEST]SDOPEN")) {
		FUNCTION = TEST_SDOPEN;
	}
	LOG_DataValid = false;
}

HAL_StatusTypeDef LOG(const char* header,const char *cmd){
	memset(log_msg,'\0',UART_RX_BUFFER_SIZE);
	strcpy(log_msg, header);
	strcat(log_msg, cmd);
	strcat(log_msg, "#");
	HAL_UART_Transmit(&huart1, (uint8_t*)log_msg, strlen(log_msg), 50);
	// CDC_Transmit_FS((uint8_t *)log_msg, strlen(log_msg));
	return HAL_OK;
}

/* Check command function */
//command CheckCommand(void){
//	//check if the data have some thing
//	if(LOG_DataValid){
//		LOG_DataValid = false;
//		if(strstr((char *)LOG_buffer, "HANDSHAKE")){
//			return INIT;
//		} else if(strstr((char *)LOG_buffer, "[VALID]")){
//			//take out number or QR
//			return VALID;
//		} else if(strstr((char *)LOG_buffer, "OPENS")){
//			return OPENS;
//		} else if(strstr((char *)LOG_buffer, "POSTPER")){
//			return POSTPER;
//		} else if(strstr((char *)LOG_buffer, "CLOSES")){
//			return CLOSES;
//		} else if(strstr((char *)LOG_buffer, "POSTDATA")){
//			return POSTDATA;
//		} else if(strstr((char *)LOG_buffer, "SLEEP")){
//			return SLEEP;
//		}
//	} else return IDLE;
//	//check rtc flag
//}


float VolumeTotal(float h) {
    float R = 12.5;
    float r = 2.0;
    float H_cone = 8.0;
    float H_cylinder = 8.0;
    float H_total = H_cone + H_cylinder;

    if (h < 0 || h > H_total) {
        return -1.0;
    }
    if (h <= H_cone) {
    	float r_h = r + ((R - r) / H_cone) * h;
        return (1.0 / 3.0) * 3.14 * h * (r_h * r_h + r_h * r + r * r);

    } else {
    	float V_cone_full = (1.0 / 3.0) * 3.14 * H_cone * (R * R + R * r + r * r);
    	float h_cylinder_part = h - H_cone;
    	float V_cylinder_part = 3.14 * R * R * h_cylinder_part;
        return (V_cone_full + V_cylinder_part)/1000; //liter unit
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, LOG_buffer, UART_RX_BUFFER_SIZE);
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, SIM_buffer, UART_RX_BUFFER_SIZE);
  WATER_InitDefault(&hi2c1);     // Water sensor driver init
  //TOF SENSOR
  statInfo_t_VL53L0X distanceStr;
  initVL53L0X(1, &hi2c1);
  uint16_t offset_DistanceTof = VL53L0X_OFFSET(&distanceStr);

  //Ultrasonic
  HAL_TIM_Base_Start(&htim1);

  // Load cellHA
  HX711_t scale1;
  HX711_t scale2;
  float calibration_factor = 200.0f;

  //Voltage
  HAL_ADC_Start(&hadc1);
  HAL_ADC_Start(&hadc2);

  //Setup
  FUNCTION = SPACE;
//  HAL_Delay(3000);

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  void SystemClock_Config(void);

  while(1) {
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); //Turn on led
	  HAL_Delay(200);
  }

  HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET);
  LOG("[DEBUG]","STM32 OK!");
  SIM_Sleep(3000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)

  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(LOG_DataValid){
//		LOG("[ECHO]",(char*)LOG_buffer);				//Echo message for testing only
		  /* Change the stage when command come in */
		  CheckHeader();
		  switch (FUNCTION) {
			  case HANDSHAKE:
        //for defbug and fail device checking
          uint16_t error_code = 0;
          char error_code_char[10];
          
				  HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_RESET); //Turn on sensor
				  initVL53L0X(1, &hi2c1);
				  HX711_Init(&scale1, LC_Data1_GPIO_Port, LC_Data1_Pin, LC_SCK_GPIO_Port, LC_SCK_Pin, calibration_factor);
				  HX711_Init(&scale2, LC_Data2_GPIO_Port, LC_Data2_Pin, LC_SCK_GPIO_Port, LC_SCK_Pin, calibration_factor);

				  if (SIM_Wakeup(3000) != 0) {
					  error_code += 1;
				  }
				  if (VL53L0X_OFFSET(&distanceStr) == -1.0f) {
					  error_code += 10;
				  }
				  if (Ultra_ReadDistance() == -1.0f) {
					  error_code += 100;
				  }
				  if((Ultra_ReadDistance() <= 20.0) && (Ultra_ReadDistance() >= 0)) {
					  char ultra_readdistance_char[50];
					  LOG("[ANS]", "FULL");
					  snprintf(ultra_readdistance_char, sizeof(ultra_readdistance_char), "%d", Ultra_ReadDistance());
					  LOG("[ANS]", ultra_readdistance_char);
				  }
				  if ((HX711_Tare(&scale1, 10, 2000) == -1.0f) || (HX711_Tare(&scale2, 10, 2000) == -1.0f)) {
					  error_code += 1000;
				  }
				  if (error_code == 0) {
					  LOG("[ANS]", "OK");
				  }else{
					  snprintf(error_code_char, sizeof(error_code_char), "%d", error_code);
					  LOG("[ANS]", "FAIL");
					  LOG("[DEBUG]", error_code_char);
					  error_code = 0;
				  }

				  SIM_Sleep(3000);
				  FUNCTION = SPACE;
				  break;

			  case VALIDATION:
				  memset(PhoneNum,'\0',20);
				  memset(data_PostSimCom,'\0',sizeof(data_PostSimCom));
				  //Prepare data
				  char* start_PhoneNum = strstr((char *)LOG_buffer, "[VALID]") + 7;
				  char* end_PhoneNum = strstr((char *)LOG_buffer, "ENDSTR");
				  strncpy(PhoneNum, start_PhoneNum, end_PhoneNum - start_PhoneNum);
				  snprintf(data_PostSimCom, sizeof(data_PostSimCom),
						  "{\"phoneNumber\":\"%s\",\"countryDialCode\":\"%s\",\"machineId\":\"%s\"}\r",
						  PhoneNum, countryDialCode, ID);
				  // LOG("DEBUG", data_PostSimCom);
				  //SIM_post
				  if (SIM_Wakeup(3000)){
					  LOG("[ANS]", "FAIL");
				  }
				  if (!SIMCom_Post(data_PostSimCom, url_identity, 5000)) {
					  //LOG("[DEBUG]",(char*)SIM_data);
					  /* User case */
					  // User have  signed in
					  if (strstr((char*)SIM_data, "\"driver\":false")) LOG("[ANS]","OK");
					  // User have not signed in
					  else if (strstr((char*)SIM_data, "\"message\":\"no user found\"")) LOG("[ANS]","REJECT");
//					  /* Driver case */
					  // driver was allow
					  else if (strstr((char*)SIM_data, "\"driver\":true")) LOG("[ANS]", "DRIVER:TRUE");
					  // driver was not allow
					  else if (strstr((char*)SIM_data, "\"message\":\"not permitted\"")) LOG("[ANS]", "DRIVER:FALSE");
					  // User case
					  else LOG("[ANS]", "FAIL");
				  }
				  SIM_Sleep(3000);
				  FUNCTION = SPACE;
				  break;

			  case POST_UCO:
				  memset(data_PostSimCom,'\0',sizeof(data_PostSimCom));
				  //prepare data
				  snprintf(data_PostSimCom, sizeof(data_PostSimCom),
						  "{\"phoneNumber\": \"%s\", \"countryDialCode\": \"%s\", \"machineId\": \"%s\", \"oil\": %.2f, \"water\": %.2f}\r",
						  PhoneNum, countryDialCode, ID, volume_oil, volume_water);
				  // LOG("DEBUG", data_PostSimCom);
				  //SIM_post
				  if (SIM_Wakeup(3000)){
					  LOG("[ANS]", "FAIL");
				  }
				  if(!SIMCom_Post(data_PostSimCom, url_postuco, 5000)) {
					  if (strstr((char *)SIM_data, "true")) {
						  LOG("[ANS]", "OK");
					  }
					  else if (strstr((char *)SIM_data, "false")) {
						  LOG("[ANS]", "REJECT");
					  }
					  else {
						  LOG("[ANS]", "UNDEFINE");
					  }
				  }
				  else {
					  LOG("[ANS]", "FAIL");
				  }
				  SIM_Sleep(3000);
				  FUNCTION = SPACE;
				  break;
			  case POST_PER:
				  memset(data_PostSimCom,'\0',sizeof(data_PostSimCom));
				  //Prepare data
				  value_VolContainer = Ultra_ReadDistance();
				  value_Voltage = Read_Voltage();
				  //Prepare data
				  snprintf(data_PostSimCom, sizeof(data_PostSimCom),
						  "{\"machineId\": \"%s\", \"volume\": %.2f, \"battery\": %.2f}\r",
						  ID, value_VolContainer, value_Voltage);
				  // LOG("DEBUG", data_PostSimCom);
				  //SIM_post
				  if (SIM_Wakeup(3000)){
					  LOG("[ANS]", "FAIL");
				  }
				  if (!SIMCom_Post(data_PostSimCom, url_postper, 5000)) {
					  if (strstr((char *)SIM_data, "true")) {
						  LOG("[ANS]", "OK");
					  }
					  else if (strstr((char *)SIM_data, "false")) {
						  LOG("[ANS]", "REJECT");
					  }
					  else {
						  LOG("[ANS]", "UNDEFINE");
					  }
				  } else {
					  LOG("[ANS]", "FAIL");
				  }
				  SIM_Sleep(3000);
				  FUNCTION = SPACE;
				  break;

			  case OPEN_INPUT:
				  /*Control motor*/
				  HAL_GPIO_WritePin(Door_IN1_GPIO_Port, Door_IN1_Pin, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(Door_IN2_GPIO_Port, Door_IN2_Pin, GPIO_PIN_RESET);
				  /*Close valve*/
				  HAL_GPIO_WritePin(VALVE_GPIO_Port, VALVE_Pin, GPIO_PIN_SET);
				  /*Transmit to header*/
				  LOG("[ANS]", "OK");
				  FUNCTION = SPACE;
				  break;
			  case CLOSE_INPUT:
				  /*Control motor*/
				  HAL_GPIO_WritePin(Door_IN1_GPIO_Port, Door_IN1_Pin, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(Door_IN2_GPIO_Port, Door_IN2_Pin, GPIO_PIN_SET);

				  /*Take value from sensor*/
				  // load cell and tof sensor
				  distance_Tof = VL53L0X_ReadDistance(&distanceStr, offset_DistanceTof); //(mm)
				  value_LoadCell = HX711_Get_Value(&scale1, 10, 2000) + HX711_Get_Value(&scale2, 10, 2000); //(gram)
				  // water sensor
				  watersensor_value = WATER_ReadHeightMM(220);   //(mm)
				  water_weight = WATER_ReadLastOn(220);     //last_on index
				  /*Turn off the sensor*/
				  HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET);
				  /*Calculating*/
//				  volume_WS = VolumeTotal(water_weight*5) + 0.025f;
				  volume_total = VolumeTotal(16 - (distance_Tof / 10)); //liter unit
				  volume_oil = (volume_total - value_LoadCell) / 0.085; //liter unit
				  volume_water = volume_total - volume_oil;
				  //Transmit to header
				  memset(data_TransmitHeader,'\0',sizeof(data_TransmitHeader));
				  snprintf(data_TransmitHeader, 40, "O%.2fW%.2fE", volume_oil, volume_water);
				  LOG("[ANS]", data_TransmitHeader);
				  // debug for water sensor
//				  if (watersensor_value < 0) {
//					  snprintf(data_TransmitHeader, sizeof(data_TransmitHeader), "WS_Value=%.2f ml", volume_WS);
//					  LOG("[DEBUG]", data_TransmitHeader);
//				  }
				  FUNCTION = SPACE;
				  break;
			  case UNLOCK_DOOR:
          /* unlock big door for driver */
				  HAL_GPIO_WritePin(LOCK_GPIO_Port, LOCK_Pin, GPIO_PIN_SET);
				  HAL_Delay(500);
				  HAL_GPIO_WritePin(LOCK_GPIO_Port, LOCK_Pin, GPIO_PIN_RESET);
				  /*Transmit to header*/
				  LOG("[ANS]", "OK");
				  FUNCTION = SPACE;
				  break;
			  case SLEEP:
				  //Turn off device
				  HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET);      //Turn off sensor source
				  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);          //Turn off the led
				  HAL_ADC_DeInit(&hadc1);
				  HAL_ADC_DeInit(&hadc2);

				  HAL_GPIO_DeInit(LOCK_GPIO_Port, LOCK_Pin);
				  HAL_GPIO_DeInit(VALVE_GPIO_Port, VALVE_Pin);
				  HAL_GPIO_DeInit(DAEGUN_LOAD_GPIO_Port, DAEGUN_LOAD_Pin);
				  //HAL_GPIO_DeInit(DEVICE_GPIO_Port, DEVICE_Pin);
				  HAL_GPIO_DeInit(LED_GPIO_Port, LED_Pin);
				  HAL_GPIO_DeInit(Door_IN1_GPIO_Port, Door_IN1_Pin);
				  HAL_GPIO_DeInit(Door_IN2_GPIO_Port, Door_IN2_Pin);
				  HAL_GPIO_DeInit(U_Trig_GPIO_Port, U_Trig_Pin);
				  HAL_GPIO_DeInit(U_Echo_GPIO_Port, U_Echo_Pin);
				  HAL_GPIO_DeInit(LC_SCK_GPIO_Port, LC_SCK_Pin);
				  HAL_GPIO_DeInit(LC_Data1_GPIO_Port, LC_Data1_Pin);
				  HAL_GPIO_DeInit(LC_Data2_GPIO_Port, LC_Data2_Pin);
				  HAL_I2C_DeInit(&hi2c1);
				  //Transmit to header
				  flag_EXTI = true;
				  LOG("[ANS]", "STM_SLEEP");
				  //Sleep
				  HAL_SuspendTick();
				  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
				  /* NEXT WAKEUP START HERE */
				  HAL_ResumeTick();
				  /* Start up peripheral device */
				  SystemClock_Config();
				  MX_USART1_UART_Init();
				  MX_USART2_UART_Init();
				  /* Start uart receive end */
				  HAL_UARTEx_ReceiveToIdle_IT(&huart1, LOG_buffer, UART_RX_BUFFER_SIZE);
				  HAL_UARTEx_ReceiveToIdle_IT(&huart2, SIM_buffer, UART_RX_BUFFER_SIZE);
				  /* Start up communication protocol */
				  MX_ADC1_Init();
				  MX_ADC2_Init();
				  MX_GPIO_Init();
				  MX_I2C1_Init();
				  /* Disable wake up flag */
				  flag_EXTI = false;
				  LOG("[ANS]", "STM_WAKE_UP");
				  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); //Turn on the led
				  break;
			  case ALARM_WAKEUP:
				  uint16_t error_code1 = 0;
				  LOG("[CMD]", "UPDATE");
				  HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_RESET); //Turn on sensor

				  initVL53L0X(1, &hi2c1);
				  HX711_Init(&scale1, LC_Data1_GPIO_Port, LC_Data1_Pin, LC_SCK_GPIO_Port, LC_SCK_Pin, calibration_factor);
				  HX711_Init(&scale2, LC_Data2_GPIO_Port, LC_Data2_Pin, LC_SCK_GPIO_Port, LC_SCK_Pin, calibration_factor);

				  if (SIM_Wakeup(3000) != 0) {
					  error_code1 += 1;
				  }
				  if (VL53L0X_OFFSET(&distanceStr) == -1.0f) {
					  error_code1 += 10;
				  }
				  if (Ultra_ReadDistance() == -1.0f) {
					  error_code1 += 100;
				  }
				  if ((HX711_Tare(&scale1, 10, 2000) == -1.0f) || (HX711_Tare(&scale2, 10, 2000) == -1.0f)) {
					  error_code1 += 1000;
				  }
				  //Prepare data
				  value_VolContainer = Ultra_ReadDistance();
				  value_Voltage = Read_Voltage();
				  //Prepare data
				  snprintf(data_PostSimCom, sizeof(data_PostSimCom),
						  "{\"machineId\": \"%s\", \"volume\": %.2f, \"battery\": %.2f}\r",
						  ID, value_VolContainer, value_Voltage);
				  // LOG("DEBUG", data_PostSimCom);
				  //SIM_post
				  if (SIM_Wakeup(3000)){
					  LOG("[ANS]", "FAIL");
				  }
				  if (!SIMCom_Post(data_PostSimCom, url_postper, 5000)) {
					  if (strstr((char *)SIM_data, "true")) {
						  LOG("[ANS]", "OK");
					  }
					  else if (strstr((char *)SIM_data, "false")) {
						  LOG("[ANS]", "REJECT");
					  }
					  else {
						  LOG("[ANS]", "UNDEFINE");
					  }
				  } else {
					  LOG("[ANS]", "FAIL");
				  }
				  SIM_Sleep(3000);
				  LOG("[ANS]", "DONE");
				  FUNCTION = SLEEP;
				  break;
			  case EXTI_WAKEUP:
				  LOG("[ANS]", "EXTI_WAKEUP");
				  FUNCTION = SPACE;
				  break;
			  case CLOSE_VALVE:
				  LOG("[ANS]", "CLOSE_VALVE");
				  HAL_GPIO_WritePin(VALVE_GPIO_Port, VALVE_Pin, GPIO_PIN_SET);
				  FUNCTION = SPACE;
				  break;
			  case OPEN_VALVE:
				  LOG("[ANS]", "OPEN_VALVE");
				  HAL_GPIO_WritePin(VALVE_GPIO_Port, VALVE_Pin, GPIO_PIN_RESET);
				  FUNCTION = SPACE;
				  break;
			  case TURN_OFF:
				  LOG("[ANS]", "TURN_OFF_MOTOR_DOOR");
				  HAL_GPIO_WritePin(Door_IN1_GPIO_Port, Door_IN1_Pin, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(Door_IN2_GPIO_Port, Door_IN2_Pin, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET); //Turn off sensor source
				  FUNCTION = SPACE;
				  break;
			  case TEST_PERCENT:
				  value_VolContainer = Ultra_ReadDistance();
				  value_Voltage = Read_Voltage();
				  //Prepare data
				  snprintf(data_PostSimCom, sizeof(data_PostSimCom), "{\"volume\": %.2f,\"battery\": %.2f}", value_VolContainer, value_Voltage);
				  //SIM_post
				  LOG("TEST", data_PostSimCom);

				  FUNCTION = SPACE;
				  break;
			  case TEST_MEASURE:
				  //Turn on the sensor
				  HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_RESET);
				  HAL_Delay(500);
				  //Take value from sensor
				  distance_Tof = VL53L0X_ReadDistance(&distanceStr, offset_DistanceTof); //(mm)
				  value_LoadCell = HX711_Get_Value(&scale1, 10, 2000) + HX711_Get_Value(&scale2, 10, 2000); //(gram)
				  float loadcell_1 = HX711_Get_Value(&scale1, 10, 2000);
				  float loadcell_2 = HX711_Get_Value(&scale2, 10, 2000);
				  // water sensor
//				  watersensor_value = WATER_ReadHeightMM(220);   //(mm)//
				  water_weight = WATER_ReadLastOn(220);     //last_on index
				  //Turn off the sensor
				  HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET);
				  //Calculate
//				  volume_WS = VolumeTotal(water_weight*5) + 0.025f;
				  volume_total = VolumeTotal(16 - (distance_Tof / 10)); //liter unit
				  volume_oil = (volume_total - value_LoadCell) / 0.085; //liter unit
				  volume_water = volume_total - volume_oil;
				  //Transmit to header
//				  snprintf(data_TransmitHeader, sizeof(data_TransmitHeader), "O%.2fW%.2fE", volume_oil, volume_water);
				  snprintf(data_TransmitHeader, sizeof(data_TransmitHeader), "Distance = %.2f mm, LoadCell_1 = %.2f g, LoadCell_2 = %.2f g;", distance_Tof, loadcell_1, loadcell_2);
				  LOG("[ANS]", data_TransmitHeader);
				  // debug for water sensor
				  snprintf(data_TransmitHeader, sizeof(data_TransmitHeader), "WS_Weight = %d", water_weight);
				  LOG("[DEBUG]", data_TransmitHeader);
				  FUNCTION = SPACE;
				  break;
			  case TEST_SDCLOSE:
				  //Control motor
				  HAL_GPIO_WritePin(Door_IN1_GPIO_Port, Door_IN1_Pin, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(Door_IN2_GPIO_Port, Door_IN2_Pin, GPIO_PIN_SET);
				  //Transmit to header
				  LOG("[ANS]", "OK");
				  FUNCTION = SPACE;
				  break;
			  case TEST_SDOPEN:
				  //Control motor
				  HAL_GPIO_WritePin(Door_IN1_GPIO_Port, Door_IN1_Pin, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(Door_IN2_GPIO_Port, Door_IN2_Pin, GPIO_PIN_RESET);
				  //Transmit to header
				  LOG("[ANS]", "OK");
				  FUNCTION = SPACE;
				  break;
			  case SPACE:
				  break;
		  }
		  memset(LOG_buffer,'\0',UART_RX_BUFFER_SIZE);		//reset receive buffer
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 12;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LOCK_Pin|VALVE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DAEGUN_LOAD_GPIO_Port, DAEGUN_LOAD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DEVICE_Pin|LED_Pin|Door_IN1_Pin|Door_IN2_Pin
                          |U_Trig_Pin|LC_SCK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LOCK_Pin VALVE_Pin */
  GPIO_InitStruct.Pin = LOCK_Pin|VALVE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : DAEGUN_LOAD_Pin */
  GPIO_InitStruct.Pin = DAEGUN_LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DAEGUN_LOAD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DEVICE_Pin */
  GPIO_InitStruct.Pin = DEVICE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEVICE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin Door_IN1_Pin Door_IN2_Pin U_Trig_Pin
                           LC_SCK_Pin */
  GPIO_InitStruct.Pin = LED_Pin|Door_IN1_Pin|Door_IN2_Pin|U_Trig_Pin
                          |LC_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : EXTI_Wakeup_Pin */
  GPIO_InitStruct.Pin = EXTI_Wakeup_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EXTI_Wakeup_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : U_Echo_Pin */
  GPIO_InitStruct.Pin = U_Echo_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(U_Echo_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LC_Data1_Pin LC_Data2_Pin */
  GPIO_InitStruct.Pin = LC_Data1_Pin|LC_Data2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	// LOG UART Handle
	if (huart->Instance == USART1) {
		LOG_DataValid = true;
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, LOG_buffer, UART_RX_BUFFER_SIZE);
	}
	// SIMCom UART Handle
	if (huart->Instance == USART2) {
		SIM_DataValid = true; // Set the flag to indicate valid SIM data
		HAL_UARTEx_ReceiveToIdle_IT(&huart2, SIM_buffer, UART_RX_BUFFER_SIZE);
	}
}

// void USB_CDC_RxHandler(uint8_t* Buf, uint32_t Len)
// {
// 	LOG_DataValid = true;
// 	memcpy(LOG_buffer, Buf, Len);
// }

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if((GPIO_Pin == GPIO_PIN_8) && flag_EXTI)
	{
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, LOG_buffer, UART_RX_BUFFER_SIZE);
		LOG_DataValid = true;
		FUNCTION = EXTI_WAKEUP;
	}
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	LOG_DataValid = true;
	FUNCTION = ALARM_WAKEUP;
	Set_RTC(0, 0, 0); //(Hour, Minute, Second)
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
