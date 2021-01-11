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
#include "skeleton.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


xQueueHandle log_queue = NULL;  // globally available


/* Logger API */
void log_add(char *log);
void log_init(uint32_t queue_length, uint32_t max_log_length);
static void uart_log_task(void *pvParameters);
void printString(char *);  // quick replacement for printf to save stack space

/*******************************************************************************
 * Logger functions
 ******************************************************************************/

void log_init(uint32_t queue_length, uint32_t max_log_length)
{   log_queue = xQueueCreate(queue_length, max_log_length);
    vQueueAddToRegistry(log_queue, "PrintQueue");
}

void uart_log_start()
{ if (xTaskCreate(uart_log_task, "uart_log_task", configMINIMAL_STACK_SIZE + 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {   printf("UART Log Task creation failed!. Reset needed.\r\n");
        while (1)
            ;
    }
}


/*!
 * @brief log_add function (works for UART or WiFi)
 */
void log_add(char *log)
{    xQueueSend(log_queue, log, 0);  // send data to back of queue,
    // non-blocking, wait=0 ==> return immediately if the queue is already full.
}

/*!
 * @brief UART log_print_task function
 */
static void uart_log_task(void *pvParameters)
{   uint32_t counter = 0;
    char log[MAX_LOG_LENGTH + 1];
    char numstring[16]; // up to 16 digits
    char *logstring ="Log ";

    while (1)
    {   xQueueReceive(log_queue, log, portMAX_DELAY);
    //    printf("Log %d: %s", counter, log); // avoid printf to save stack space
        printString(logstring);
        itoa(counter,numstring,10); // non standard but it is in <stdlib.h>
        printString(numstring);
        fputc(' ',stdout);
        printString(log);
        counter++;
    }
}

void printString(char *string)
{   int i=0;
    while (string[i] != '\0') 
        {
            fputc(string[i],stdout); // print single character, avoid printf to save on stack space and speed up
            i++;
        }    
}