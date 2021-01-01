/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define BLINK_GPIO 13 // IO13

/* prototypes */
static void heartbeatTask(void *pvParameters);
void start_heartbeat(void);

void start_heartbeat()
{ TaskHandle_t heartbeat_handle = NULL;
    // TaskFunction_t pvTaskCode, const char * const pcName,  configSTACK_DEPTH_TYPE usStackDepth,
    //  void *pvParameters, UBaseType_t uxPriority,  TaskHandle_t *pxCreatedTask (optional)
    if(xTaskCreate(heartbeatTask, "heartbeat", 2048, NULL, tskIDLE_PRIORITY + 1, &heartbeat_handle) !=pdPASS)
    {   printf("Heartbeat Task creation failed! reboot needed.\r\n");
        while (1); // hang indefinitely
    }
    
}

/* just have a task execute continually blinking LED. This should keep task watch dog timer restting */

void heartbeatTask(void *pvParameters)
{
    /* Configure the IOMUX register for pad BLINK_GPIO 
    Huzzah32 uses IO13 according to schematic
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
	printf("Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
	printf("Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_task_wdt_reset(); // reset task watchdog
    }
}
