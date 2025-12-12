//======================================================
// SLEEP.c
//======================================================

#include "Inc/Sleep.h"

//======================================================
// SLEEP 모드 처리
//======================================================
void SleepMode(void)
{
    //--------------------------------------------------
    // 1) Peripheral OFF
    //--------------------------------------------------
    HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET);   // Sensor OFF
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);       // LED OFF

    HAL_ADC_DeInit(hadc1);
    HAL_ADC_DeInit(hadc2);

    HAL_GPIO_DeInit(LOCK_GPIO_Port, LOCK_Pin);
    HAL_GPIO_DeInit(VALVE_GPIO_Port, VALVE_Pin);
    HAL_GPIO_DeInit(DAEGUN_LOAD_GPIO_Port, DAEGUN_LOAD_Pin);
    HAL_GPIO_DeInit(LED_GPIO_Port, LED_Pin);
    HAL_GPIO_DeInit(Door_IN1_GPIO_Port, Door_IN1_Pin);
    HAL_GPIO_DeInit(Door_IN2_GPIO_Port, Door_IN2_Pin);
    HAL_GPIO_DeInit(U_Trig_GPIO_Port, U_Trig_Pin);
    HAL_GPIO_DeInit(U_Echo_GPIO_Port, U_Echo_Pin);
    HAL_GPIO_DeInit(LC_SCK_GPIO_Port, LC_SCK_Pin);
    HAL_GPIO_DeInit(LC_Data1_GPIO_Port, LC_Data1_Pin);
    HAL_GPIO_DeInit(LC_Data2_GPIO_Port, LC_Data2_Pin);

    HAL_I2C_DeInit(hi2c1);

    //--------------------------------------------------
    // 2) Sleep 전 상태 출력
    //--------------------------------------------------
    flag_EXTI = true;
    LOG("[ANS]", "STM_SLEEP");

    //--------------------------------------------------
    // 3) STOP 모드 진입
    //--------------------------------------------------
    HAL_SuspendTick();
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    //--------------------------------------------------
    // =======  여기서 EXTI 인터럽트로 깨움  =======
    //--------------------------------------------------

    //--------------------------------------------------
    // 4) Wake-up 처리
    //--------------------------------------------------
    HAL_ResumeTick();

    // 시스템 클록 재설정
    SystemClock_Config();

    //--------------------------------------------------
    // 5) Core peripheral 재초기화
    //--------------------------------------------------
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    HAL_UARTEx_ReceiveToIdle_IT(huart1, LOG_buffer, UART_RX_BUFFER_SIZE);
    HAL_UARTEx_ReceiveToIdle_IT(huart2, SIM_buffer, UART_RX_BUFFER_SIZE);

    //--------------------------------------------------
    // 6) Sensor / ADC / I2C / GPIO 재초기화
    //--------------------------------------------------
    MX_ADC1_Init();
    MX_ADC2_Init();
    MX_GPIO_Init();
    MX_I2C1_Init();

    //--------------------------------------------------
    // 7) Wakeup 완료
    //--------------------------------------------------
    flag_EXTI = false;
    LOG("[ANS]", "STM_WAKE_UP");

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);     // LED ON
}


//======================================================
//                    LEGACY CODE
//======================================================
//Turn off device
//HAL_GPIO_WritePin(DEVICE_GPIO_Port, DEVICE_Pin, GPIO_PIN_SET); //Turn off sensor source
//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); //Turn off the led
//HAL_ADC_DeInit(&hadc1);
//HAL_ADC_DeInit(&hadc2);
//
//HAL_GPIO_DeInit(LOCK_GPIO_Port, LOCK_Pin);
//HAL_GPIO_DeInit(VALVE_GPIO_Port, VALVE_Pin);
//HAL_GPIO_DeInit(DAEGUN_LOAD_GPIO_Port, DAEGUN_LOAD_Pin);
////HAL_GPIO_DeInit(DEVICE_GPIO_Port, DEVICE_Pin);
//HAL_GPIO_DeInit(LED_GPIO_Port, LED_Pin);
//HAL_GPIO_DeInit(Door_IN1_GPIO_Port, Door_IN1_Pin);
//HAL_GPIO_DeInit(Door_IN2_GPIO_Port, Door_IN2_Pin);
//HAL_GPIO_DeInit(U_Trig_GPIO_Port, U_Trig_Pin);
//HAL_GPIO_DeInit(U_Echo_GPIO_Port, U_Echo_Pin);
//HAL_GPIO_DeInit(LC_SCK_GPIO_Port, LC_SCK_Pin);
//HAL_GPIO_DeInit(LC_Data1_GPIO_Port, LC_Data1_Pin);
//HAL_GPIO_DeInit(LC_Data2_GPIO_Port, LC_Data2_Pin);
//HAL_I2C_DeInit(&hi2c1);
////Transmit to header
//flag_EXTI = true;
//LOG("[ANS]", "STM_SLEEP");
////Sleep
//HAL_SuspendTick();
//HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,
//PWR_STOPENTRY_WFI);
///* NEXT WAKEUP START HERE */
//HAL_ResumeTick();
///* Start up peripheral device */
//SystemClock_Config();
//MX_USART1_UART_Init();
//MX_USART2_UART_Init();
///* Start uart receive end */
//HAL_UARTEx_ReceiveToIdle_IT(&huart1, LOG_buffer,
//UART_RX_BUFFER_SIZE);
//HAL_UARTEx_ReceiveToIdle_IT(&huart2, SIM_buffer,
//UART_RX_BUFFER_SIZE);
///* Start up communication protocol */
//MX_ADC1_Init();
//MX_ADC2_Init();
//MX_GPIO_Init();
//MX_I2C1_Init();
///* Disable wake up flag */
//flag_EXTI = false;
//LOG("[ANS]", "STM_WAKE_UP");
//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); //Turn on the led
