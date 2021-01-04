/* LEDC (LED Controller) fade example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "skeleton.h"

/*
 * About this example
 *
 * 1. Start with initializing LEDC module:
 *    a. Set the timer of LEDC first, this determines the frequency
 *       and resolution of PWM.
 *    b. Then set the LEDC channel you want to use,
 *       and bind with one of the timers.
 *
 * 2. You need first to install a default fade function,
 *    then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example uses GPIO18/19/4/5 as LEDC output,
 *    and it will change the duty repeatedly.
 *
 * 5. GPIO18/19 are from high speed channel group.
 *    GPIO4/5 are from low speed channel group.
 *
 */

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (18)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (19)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1

#define LEDC_TEST_CH_NUM       (2)
#define LEDC_FREQUENCY         (5000)

void ledc_example_init();
void set_ledc_pwm(uint32_t *duty);
void stop_ledc_pwm();

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

void ledc_example_init()
{
    // Initialize the ledc controller with a timer
    ledc_timer_config_t ledc_timer = {
        .duty_resolution    = LEDC_TIMER_13_BIT,        // resolution of PWM duty
        .freq_hz            = LEDC_FREQUENCY,           // frequency of PWM signal
        .speed_mode         = LEDC_HS_MODE,             // timer mode
        .timer_num          = LEDC_HS_TIMER,            // timer index
        .clk_cfg            = LEDC_AUTO_CLK,            // Auto select the source clock
    };
    ledc_timer_config(&ledc_timer);

    // if (ledc_timer_config(&ledc_timer) != ESP_OK)
    // {
    //     printf("PWM Timer failed to start. Need reset!\n");
    //     while (1)
    //     {
    //         ;
    //     }
    // }

    // Initialize the ledc controller with channel

    ledc_channel_config(&ledc_channel[0]);
    ledc_channel_config(&ledc_channel[1]);

    // for ( int i = 0; i < LEDC_TEST_CH_NUM; i++ )
    // {
    //     if (ledc_channel_config(&ledc_channel[i]) != ESP_OK)
    //     {
    //         printf("PWM Channel failed to start. Need reset!\n");
    //         while (1)
    //         {
    //             ;
    //         }
    //     }
    // }
}

void set_ledc_pwm(uint32_t *duty)
{
    for ( int ch = 0 ; ch < LEDC_TEST_CH_NUM; ch++ )
    {
        ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, duty[ch]);
        ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
    }
}

void stop_ledc_pwm()
{
    for ( int ch = 0 ; ch < LEDC_TEST_CH_NUM; ch++ )
    {
        ledc_stop(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 0);
    }
}