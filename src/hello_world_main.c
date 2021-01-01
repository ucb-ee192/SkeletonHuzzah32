/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "skeleton.h"

void start_timer(void);  // prototype
void start_control(void);  // prototype
void log_init(uint32_t, uint32_t);
void print_tasks(void);
void start_heartbeat(void);

void app_main()
{   double z=3.14159;
   
    printf("Hello world! 12/28/20 v4\n");
    printf("Floating point 3.14159=%10.3f\n", z);
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

/* welcome message */
    printf("EE192 Spring 2021 30 Dec 2020 v0.0\n\r");

/* Initialize logger for 32 entries with maximum length. Start first as needed for control logging */
    printf("About to start logging task\n");
    log_init(32, MAX_LOG_LENGTH); // buffer up to 32 lines of text

    printf("About to start timer apps\n");
    start_timer();
    printf("About to start control task\n");
    start_control();
    printf("About to start heartbeat task\n");
    start_heartbeat();
    print_tasks(); // verify what tasks are running

    
    vTaskSuspend(NULL);  // suspend current task
    
    printf("Echoing character input [x to dump core]:");
    while(1) {
		uint8_t ch;
	    ch = fgetc(stdin);
	    if (ch!=0xFF)
	    {
		    fputc(ch, stdout);
	    }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // allow IDLE process to run, else get watchdog timer
        if (ch=='x')
        {   printf("About to dump core\n");
            fflush(stdout);
            assert(0); // trigger core dump
        }
    }

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

/* for debugging see what tasks are doing */
void print_tasks()
{   UBaseType_t num_tasks;
    char *task_stats;

    num_tasks =  uxTaskGetNumberOfTasks();
    printf("# of tasks %d\n", (int) num_tasks);
 /* can't put this buffer  on stack, causes overflow, use heap */
    task_stats = (char *) malloc(num_tasks*64); // allow 64 char per task?
    // Check if the memory has been successfully  allocated by malloc or not 
    if (task_stats == NULL) { 
        printf("Error in print_tasks: Memory not allocated.\n"); 
        exit(0); 
    } 
    
    vTaskGetRunTimeStats(task_stats);
    printf("%s\n", task_stats);
    printf("vTaskList:\n");
    printf("Name          State   Priority  Stack   Task#\n");
    vTaskList(task_stats);
    printf("%s\n", task_stats);
    free(task_stats);
}

