/* Timer group-hardware timer example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/* modified from 
.platformio/packages/framework-espidf/examples/peripherals/timer_group/main/timer_group_example_main.c
by R. Fearing 12/20
use single timer without reload
*/

#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "esp_task_wdt.h"
#include "skeleton.h"  /* parameters used by multiple files, defined in one place */

#define TIMER_DIVIDER         16  //  Hardware timer clock divider on 80 MHz clock
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
/* if timer interval is longer than CONFIG_ESP_TASK_WDT_TIMEOUT_S, will get a wathcdog timeout */
#define TIMER_INTERVAL0_SEC   (2.5) // sample test interval for the first timer
#define TEST_WITHOUT_RELOAD   0        // testing will be done without auto reload
#define TEST_WITH_RELOAD      1        // testing will be done with auto reload
#define TWDT_TIMEOUT_S          5
#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                printf("TWDT ERROR\n");                                \
                abort();                                               \
            }                                                          \
})


/*
 * A sample structure to pass events
 * from the timer interrupt handler to the main program.
 */
typedef struct {
    int type;  // the type of timer's event
    int timer_group;
    int timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

xQueueHandle timer_queue;


/*
 * Timer group0 ISR handler
 *
 * Note:
 * We don't call the timer API here because they are not declared with IRAM_ATTR.
 * If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
 * we can allocate this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API.
 */
void IRAM_ATTR timer_group0_isr(void *para)
{
    timer_spinlock_take(TIMER_GROUP_0);
    int timer_idx = (int) para;

    /* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
    uint32_t timer_intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, timer_idx);

    /* Prepare basic event data
       that will be then sent back to the main program task */
    timer_event_t evt;
    evt.timer_group = 0;
    evt.timer_idx = timer_idx;
    evt.timer_counter_value = timer_counter_value;

    /* Clear the interrupt
       and update the alarm time for the timer */
    if (timer_intr & TIMER_INTR_T0) 
    {   evt.type = TEST_WITHOUT_RELOAD;
        timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
        timer_counter_value += (uint64_t) (TIMER_INTERVAL0_SEC * TIMER_SCALE);
        timer_group_set_alarm_value_in_isr(TIMER_GROUP_0, timer_idx, timer_counter_value);
    } 
    else
    { evt.type = -1; // not supported event type
    }

    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, timer_idx);
    timer_spinlock_give(TIMER_GROUP_0); // release timer in case needed by other functions
    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(timer_queue, &evt, NULL);  
}

/*
 * Initialize selected timer of the timer group 0
 *
 * timer_idx - the timer number to initialize
 * auto_reload - should the timer auto reload on alarm?
 * timer_interval_sec - the interval of alarm to set
 */
static void example_tg0_timer_init(int timer_idx,
                                   bool auto_reload, double timer_interval_sec)
{    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = auto_reload,
    }; // default clock source is APB
    timer_init(TIMER_GROUP_0, timer_idx, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
                       (void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, timer_idx);
}

/*
 * The main task of this example program
 */
static void timer_example_evt_task(void *arg)
{   timer_event_t evt;
    uint64_t task_counter_value, time_elapsed;
    const TickType_t xDelay = 50 / portTICK_PERIOD_MS;
 //   UBaseType_t num_messages;
    while (1) {   
        
        while(uxQueueMessagesWaiting(timer_queue) < (UBaseType_t)1)
        { taskYIELD(); // may need another task for this to do anything
            // add idle so watch dog not triggered 
         vTaskDelay( xDelay );  /* Block for 50ms. */
         esp_task_wdt_reset(); // keep watchdog from triggering due to this task
        }

        xQueueReceive(timer_queue, &evt, portMAX_DELAY); // assume only 1 reader for this queue
        timer_get_counter_value(evt.timer_group, evt.timer_idx, &task_counter_value);
        //printf("EVENT TIME %llu ------ TASK TIME %llu\n", 
        //    evt.timer_counter_value, task_counter_value);

        time_elapsed = task_counter_value - evt.timer_counter_value;
        printf("Current time: %8.3f s. Elapsed counts since alarm event: %llu and msec: %.6f ms\n",
             (double) task_counter_value / TIMER_SCALE, time_elapsed, 1000.0*(double) time_elapsed / TIMER_SCALE); // long long unsigned int
        
       
    }
}

/*
 * In this example, we will test hardware timer0 and timer1 of timer group0.
 */
void start_timer(void)
{
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    example_tg0_timer_init(TIMER_0, TEST_WITHOUT_RELOAD, TIMER_INTERVAL0_SEC);
    // TaskFunction_t pvTaskCode, const char * const pcName,  configSTACK_DEPTH_TYPE usStackDepth,
    //  void *pvParameters, UBaseType_t uxPriority,  TaskHandle_t *pxCreatedTask (optional)
    xTaskCreate(timer_example_evt_task, "timer_evt_task", 2048, NULL, 2, NULL);
    //printf("Initialize TWDT\n");
    //Initialize or reinitialize TWDT
    //CHECK_ERROR_CODE(esp_task_wdt_init(TWDT_TIMEOUT_S, false), ESP_OK);
    /* watch dog timer initialization already done by startup code */
    //esp_task_wdt_add(timer_example_evt_task);
    //esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(0));
    //esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(1));  // add idle tasks
    //CHECK_ERROR_CODE(esp_task_wdt_deinit(), ESP_OK); // try to turn off here
}

