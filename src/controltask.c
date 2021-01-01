/* template for main control loop: read sensor, caclulate control, output commands, log data */
#include <stdio.h>
#include <math.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "skeleton.h"  /* parameters used by multiple files, defined in one place */
/* globals */
extern volatile uint32_t systime; //systime updated very 100 us = 4 days ==> NEED OVERFLOW protection
extern float sqrt_array[1000]; // to hold results
extern uint32_t ulIdleCycleCount;

/*******************************************************************************
 * Definitions
 ******************************************************************************/



/* definitions for dynamic simulation OMEGAD is damped frequency */
#define SIGMA (1.0/5)
#define PHI 0.5
#define OMEGAD 1.0   


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
    

void start_control(void)
{
    // TaskFunction_t pvTaskCode, const char * const pcName,  configSTACK_DEPTH_TYPE usStackDepth,
    //  void *pvParameters, UBaseType_t uxPriority,  TaskHandle_t *pxCreatedTask (optional)
    if(xTaskCreate(control_task, "control_task", 2048, NULL, tskIDLE_PRIORITY + 2, NULL) !=pdPASS)
    {   printf("Control Task creation failed! reboot needed.\r\n");
        while (1); // hang indefinitely
    }
}

/*!
 * @brief write_task_1 function
 */
static void control_task(void *pvParameters)
{
    char log[MAX_LOG_LENGTH + 1];
    TickType_t tick_start, tick_end, tick_now;
    uint64_t task_counter_value;
    double runtime, starttime;
   
    int i;
    const TickType_t xDelay1000ms = pdMS_TO_TICKS( 1000 );
    double y; // output
/* do time in double for simplicity. May check to see if too slow */
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);
    starttime = ((double) task_counter_value / TIMER_SCALE);
    tick_start = xTaskGetTickCount();

    for (i = 0; i < 10; i++)
    {   tick_now = xTaskGetTickCount();
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);
        runtime = ((double) task_counter_value / TIMER_SCALE) -starttime;
        /* pseudo code: read_sensors() */
        /* pseudo code: calculate steering and motor PWM */
        /* output steering angle and motor PWM */
        /* simulate a dynamic response */
        y=exp(-SIGMA*runtime)*cos(OMEGAD*runtime-PHI);

     	sprintf(log, "control: tick %d  time %8.3f (s) y=%8.3f\n\r",
        		(int) tick_now, runtime, y);
        log_add(log);
       

        vTaskDelay(xDelay1000ms); // relative delay in ticks
 //     vTaskDelayUntil( &tick_start, xDelay1000ms );  // unblocks at absolute time- needed for periodic functions
        taskYIELD();
    }
    
    tick_end = xTaskGetTickCount();
    sprintf(log, "Control Task done. tick_start %d tick_end %d\n\r", (int) tick_start, (int) tick_end);
    log_add(log);
    sprintf(log, "Control Task. xTaskGetTickCount=%ld\n\r",
        (long) xTaskGetTickCount() );
    //		(long)ulIdleCycleCount);
        log_add(log);

    vTaskDelay(1000 / portTICK_PERIOD_MS);     // give time for log to finish before suspending 
    print_tasks();
    
    vTaskSuspend(NULL);  // suspend current task
}
