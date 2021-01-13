/* Control Script
EE 192, Spring 2021, R. Fearing

ENTER DESCRIPTION

*/

// Includes
#include <stdio.h>
#include <math.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "skeleton.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* definitions for dynamic simulation OMEGAD is damped frequency */
#define SIGMA (1.0/5)
#define PHI 0.5
#define OMEGAD 1.0   

/* globals */
extern volatile uint32_t systime; //systime updated very 100 us = 4 days ==> NEED OVERFLOW protection
extern float sqrt_array[1000]; // to hold results
extern uint32_t ulIdleCycleCount;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Application API */
static void control_task(void *pvParameters);
void start_control(void);

/* Logger API */
extern void log_add(char *log);
extern void log_task(void *pvParameters);
extern void print_tasks(void);

/* PWM API */
extern void ledc_example_init();
extern void set_ledc_pwm(uint32_t*);
extern void stop_ledc_pwm();

/*******************************************************************************
 * Functions
 ******************************************************************************/

void start_control(void)
{
    // TaskFunction_t pvTaskCode, const char * const pcName,  configSTACK_DEPTH_TYPE usStackDepth,
    //  void *pvParameters, UBaseType_t uxPriority,  TaskHandle_t *pxCreatedTask (optional)
    if(xTaskCreate(control_task, "control_task", 2048, NULL, tskIDLE_PRIORITY + 5, NULL) !=pdPASS)
    {   
        printf("Control Task creation failed! reboot needed.\r\n");
        while (1);
    }
}

void ESC_startup(void)
{
    uint32_t duty[2] = {0, 4095};
    // Send pulse
    set_ledc_pwm(duty);
    vTaskDelay(pdMS_TO_TICKS(60));
    
    duty[1] = 0;
    set_ledc_pwm(duty);
    vTaskDelay(pdMS_TO_TICKS(200));

    duty[1] = 200;
    set_ledc_pwm(duty);
}

// Control task that handles vehicle dynamics and control
static void control_task(void *pvParameters)
{   
    char log[MAX_LOG_LENGTH + 1];
    TickType_t tick_start, tick_end, tick_now;
    uint64_t task_counter_value, task_counter_value1;
    double runtime, starttime, checktime;
    int i;
    const TickType_t xDelay1000ms = pdMS_TO_TICKS( 1000 );
    double y; // output

    // Initialize pwm
    uint32_t duty[2] = {0, 200};
    ledc_example_init();

    // Use timer
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);
    starttime = ((double) task_counter_value / TIMER_SCALE);
    tick_start = xTaskGetTickCount();

    // Testing different print timings
    // measure time log_add() takes (time between timer_get_counter_value)
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);
    //   snprintf(log, sizeof(log), "Starting control: at tick %d  current task counter value %ld\n\r",
    //       		(int) tick_start, (long) task_counter_value);
    itoa((int)task_counter_value,log,10);
    log_add(log);
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value1);
    
    runtime = ((double) task_counter_value / TIMER_SCALE);       // do floating point after timing     
    checktime = ((double) task_counter_value1 / TIMER_SCALE);
    
    snprintf(log, sizeof(log), "log_add() took %8.3f milliseconds (s)\n\r",
        		1000*(checktime-runtime));
    
    // Add to log queue
    log_add(log);

    // Begin ESC startup callibration routine
    ESC_startup();

    for (i = 0; i < 10; i++)
    {   
        tick_now = xTaskGetTickCount();
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);
        runtime = ((double) task_counter_value / TIMER_SCALE) -starttime;

        /****************************************************************************
        Control Tasks
        ****************************************************************************/
        /* pseudo code: read_sensors() */
        /* pseudo code: calculate steering and motor PWM */
        /* output steering angle and motor PWM */
        /* simulate a dynamic response */

        y=exp(-SIGMA*runtime)*cos(OMEGAD*runtime-PHI);
        // defensive programming, make sure string fits in "log"
     	snprintf(log, sizeof(log), "control: tick %d  time %8.3f (s) y=%8.3f\n\r",
        		(int) tick_now, runtime, y);
        
        // Add to log queue
        log_add(log);

        // Set duty cycle of PWMs
        duty[1] += 10;
        set_ledc_pwm(duty);

        vTaskDelay(xDelay1000ms); // relative delay in ticks

        taskYIELD();
    }

    // Stop PWM
    stop_ledc_pwm();
    
    // For debugging control tasks
    tick_end = xTaskGetTickCount();
    sprintf(log, "Control Task done. tick_start %d tick_end %d\n\r", (int) tick_start, (int) tick_end);
    log_add(log);
    sprintf(log, "Control Task. xTaskGetTickCount=%ld\n\r",
        (long) xTaskGetTickCount() );
    log_add(log);

    vTaskDelay(1000 / portTICK_PERIOD_MS);     // give time for log to finish before suspending 
    print_tasks();
    
    vTaskSuspend(NULL);  // suspend current task
}
