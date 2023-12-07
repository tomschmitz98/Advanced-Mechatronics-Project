
#include "button_io.h"
#include "core_m4.h"
#include "gpio.h"
#include "motor.h"
#include "productDef.h"
#include "reaction.h"
#include "sensors.h"
#include "serial.h"
#include "slapper.h"
#include "stdio.h"
#include "stm_rcc.h"
#include "stm_utils.h"
#include "timers.h"

// TODO: Switch button IO to appropriate pins
// TODO: Transfer motor stuff into project

#define ALLOW_PREEMPTION true

#if ALLOW_PREEMPTION
#define CONTINUE continue
#else
#define CONTINUE
#endif

volatile uint32_t gEvents = 0;

const gpio_config_t led0_configs = {14,     0,         LOW,        bank_b,
                                    output, push_pull, high_speed, no_pull};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

void init(void) {
    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    USB_GPIO_Init();
    MX_USART3_UART_Init();
    MX_USB_OTG_FS_PCD_Init();

    init_gpio(led0_configs);

    if (check_clock_flag(SOFTWARE_RESET)) {
        clear_clock_flags();
        setPin(bank_b, 14);
    }
    clear_clock_flags();

    initialize_ir_sensors();
    initialize_fsr();
    init_buttons();
    init_motor_pins();
    // init_motor_timer();
    // init_pid_timer();
    config_reaction();

    init_heartbeat();
}

static void peform_slapper_action(slapper_action_t action) {
    static slapper_action_t prevAction = NO_ACTION;
    static uint32_t user_score = 0, cpu_score = 0;

    if (action == prevAction) {
        return;
    }

    switch (action) {
    case NO_ACTION:
        // Do nothing
        break;
    case SENSORS_USER_QUERY:
        printf("Please place your hand in the proper location\r\n");
    case PRINT_PAUSED:
        printf("Game paused. To resume, please press the start button\r\n");
        break;
    case PRINT_RED:
        printf("Wait for it...\r\n");
        break;
    case START_REACTION:
        printf("Now!!!\r\n");
        start_reaction();
        break;
    case START_ACTUATOR:
        start_slap();
        break;
    case RESET_ACTUATOR:
        reset_slap();
        break;
    case UPDATE_SCORE_CPU:
        cpu_score++;
        break;
    case UPDATE_SCORE_USER:
        user_score++;
        break;
    case QUERY_PLAY_AGAIN:
        block_actuation_events();
        printf("User score: %lu, CPU score: %lu\r\n", user_score, cpu_score);
        printf("Press start to play again\r\n");
        break;
    default:
        assert(false);
    }

    prevAction = action;
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    bool actuation_done = false, start_btn, pause_btn;
    slapper_action_t action = NO_ACTION;
    init();

    printf("Welcome to this reaction time game.\r\n");
    printf("Are you confident enough in your reflexes to press the start "
           "button?\r\n");

    while (1) {
        // Highest Priority
        if (gEvents & E_HEARTBEAT) {
            // run state machine for game
            start_btn = button_changed_state(START_BUTTON);
            pause_btn = button_changed_state(PAUSE_BUTTON);
            action = run_slapper(start_btn, pause_btn, actuation_done);
            peform_slapper_action(action);
            actuation_done = false;
            gEvents &= E_HEARTBEAT;
            CONTINUE;
        }
        if (gEvents & E_REACTION) {
            printf("Reaction time: %lu ms\r\n", read_reaction());
            gEvents &= ~E_REACTION;
            CONTINUE;
        }
        if (gEvents & E_ACTUATION_DONE) {
            actuation_done = done_with_actuation();
            gEvents &= ~E_ACTUATION_DONE;
            CONTINUE;
        }
        // Lowest Priority
    }

    return 1;
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
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    RCC_OscInitStruct.PLL.PLLR = 2;
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

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
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
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
