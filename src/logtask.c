/* print logging using queue

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MAX_LOG_LENGTH 64

static xQueueHandle log_queue = NULL; 


/* Logger API */
void log_add(char *log);
void log_init(uint32_t queue_length, uint32_t max_log_length);
static void log_task(void *pvParameters);


/*******************************************************************************
 * Logger functions
 ******************************************************************************/
void log_init(uint32_t queue_length, uint32_t max_log_length)
{
    log_queue = xQueueCreate(queue_length, max_log_length);
    if (xTaskCreate(log_task, "log_task", configMINIMAL_STACK_SIZE + 166, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {   printf("Task creation failed!. Reset needed.\r\n");
        while (1)
            ;
    }
    vQueueAddToRegistry(log_queue, "PrintQueue");
}


/*!
 * @brief log_add function
 */
void log_add(char *log)
{
    xQueueSend(log_queue, log, 0);  // send data to back of queue,
    // non-blocking, wait=0 ==> return immediately if the queue is already full.
}

/*!
 * @brief log_init function
 */

/*!
 * @brief log_print_task function
 */
static void log_task(void *pvParameters)
{   uint32_t counter = 0;
    char log[MAX_LOG_LENGTH + 1];
    while (1)
    {   xQueueReceive(log_queue, log, portMAX_DELAY);
        printf("Log %d: %s", counter, log);
        counter++;
    }
}
