/* LEDC PWM Script
EE 192, Spring 2021, R. Fearing

ENTER DESCRIPTION

*/

// Includes
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "skeleton.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (13)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (19)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1

#define LEDC_TEST_CH_NUM       (2)
#define LEDC_FREQUENCY         (50)


static ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
        {
            .channel     = LEDC_HS_CH0_CHANNEL,
            .duty        = 0,
            .gpio_num    = LEDC_HS_CH0_GPIO,
            .speed_mode  = LEDC_HS_MODE,
            .hpoint      = 0,
            .timer_sel   = LEDC_HS_TIMER
        },
        {
            .channel     = LEDC_HS_CH1_CHANNEL,
            .duty        = 0,
            .gpio_num    = LEDC_HS_CH1_GPIO,
            .speed_mode  = LEDC_HS_MODE,
            .hpoint      = 0,
            .timer_sel   = LEDC_HS_TIMER
        }
    };

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void ledc_example_init();
void set_ledc_pwm(uint32_t *duty);
void stop_ledc_pwm();
void perform_ledc_fade(int time);

/*******************************************************************************
 * Functions
 ******************************************************************************/

// Initialize ledc controller with defined config
void ledc_example_init()
{
    // Initialize the ledc controller with a timer
    ledc_timer_config_t ledc_timer = {
        .duty_resolution    = LEDC_TIMER_12_BIT,        // resolution of PWM duty
        .freq_hz            = LEDC_FREQUENCY,           // frequency of PWM signal
        .speed_mode         = LEDC_HS_MODE,             // timer mode
        .timer_num          = LEDC_HS_TIMER,            // timer index
        .clk_cfg            = LEDC_AUTO_CLK,            // Auto select the source clock
    };

    if (ledc_timer_config(&ledc_timer) != ESP_OK)
    {
        printf("PWM Timer failed to start. Need reset!\n");
        while (1)
        {
            ;
        }
    }

    // Initialize the ledc controller with channel
    for ( int ch = 0 ; ch < LEDC_TEST_CH_NUM; ch++ )
    {
        if (ledc_channel_config(&ledc_channel[ch]) != ESP_OK)
        {
            printf("PWM Channel failed to start. Need reset!\n");
            while (1)
            {
                ;
            }
        }
    }
}

// Function for setting ledc pwm duty cycles
void set_ledc_pwm(uint32_t *duty)
{
    for ( int ch = 0 ; ch < LEDC_TEST_CH_NUM; ch++ )
    {
        ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, duty[ch]);
        ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
    }
}

void perform_ledc_fade(int time)
{
    ledc_set_fade_with_time(ledc_channel[0].speed_mode,
            ledc_channel[0].channel, 4000, time);
    ledc_fade_start(ledc_channel[0].speed_mode,
            ledc_channel[0].channel, LEDC_FADE_NO_WAIT);
    
    vTaskDelay(time / portTICK_PERIOD_MS);

    ledc_set_fade_with_time(ledc_channel[0].speed_mode,
            ledc_channel[0].channel, 0, time);
    ledc_fade_start(ledc_channel[0].speed_mode,
            ledc_channel[0].channel, LEDC_FADE_NO_WAIT);
    
    vTaskDelay(time / portTICK_PERIOD_MS);
}

// Function for ceasing all pwm functions
void stop_ledc_pwm()
{
    for ( int ch = 0 ; ch < LEDC_TEST_CH_NUM; ch++ )
    {
        ledc_stop(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 0);
    }
}