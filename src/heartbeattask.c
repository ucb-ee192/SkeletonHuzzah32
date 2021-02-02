/* Heartbeat Script
EE 192, Spring 2021, R. Fearing

ENTER DESCRIPTION

*/

// Includes
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BLINK_GPIO 13 // IO13

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void heartbeatTask(void *pvParameters);
void start_heartbeat(void);

/*******************************************************************************
 * Functions
 ******************************************************************************/

void start_heartbeat()
{ 
    TaskHandle_t heartbeat_handle = NULL;
    // TaskFunction_t pvTaskCode, const char * const pcName,  configSTACK_DEPTH_TYPE usStackDepth,
    //  void *pvParameters, UBaseType_t uxPriority,  TaskHandle_t *pxCreatedTask (optional)
    if(xTaskCreate(heartbeatTask, "heartbeat", 2048, NULL, tskIDLE_PRIORITY + 1, &heartbeat_handle) !=pdPASS)
    {   
        printf("Heartbeat Task creation failed! reboot needed.\r\n");
        while (1);
    }
    
}

// Task executes that continually blinks LED. This should keep task watch dog timer restting
void heartbeatTask(void *pvParameters)
{
    /* Configure the IOMUX register for pad BLINK_GPIO 
    Huzzah32 uses IO13 according to schematic
    */
    // gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    // gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        // gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        // gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_task_wdt_reset(); // reset task watchdog
    }
}
