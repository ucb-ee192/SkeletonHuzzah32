/* MC PWM Script
EE 192, Spring 2021, R. Fearing

ENTER DESCRIPTION

*/

// Includes
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "skeleton.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void mcpwm_example_gpio_initialize();
void set_mcpwm_throttle();

/*******************************************************************************
 * Functions
 ******************************************************************************/

void mcpwm_example_gpio_initialize(void)
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 18);    //Set GPIO 18 as PWM0A, to which servo is connected

    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
}


void set_mcpwm_throttle(uint32_t *throttle)
{
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, throttle);
}
