/* Main Script
EE 192, Spring 2021, R. Fearing

ENTER DESCRIPTION

*/

// Includes
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "skeleton.h"

// Define macros
#define WIFILOG

// Function prototypes
void start_timer(void);
void start_control(void);
void log_init(uint32_t, uint32_t);
void uart_log_start(void);
void print_tasks(void);
void start_heartbeat(void);
void start_user(void);
void wifi_start(void);

// Main application
void app_main()
{   
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

    /* Welcome message */
    printf("EE192 Spring 2021 09 Jan 2021 v0.1\n\r");
    /* Initialize logger queue for 32 entries with maximum length. 
    * Start first as needed for control logging */
    log_init(32, MAX_LOG_LENGTH); 
    // use log_add(char *log) to send messages to either UART or WiFi.
    // log_add uses a queue so that debugging should not block real time control routines
#ifdef WIFILOG
    printf("Starting Wifi Tasks\n");
    wifi_start();
#endif
#ifndef WIFILOG
    printf("About to start UART logging task\n");
    uart_log_start(); // start UART log
#endif
    printf("About to start timer apps\n");
    start_timer();
    printf("About to start control task\n");
    start_control();
    printf("About to start heartbeat task\n");
    start_heartbeat();
    printf("About to start user task\n");
    start_user();

    // Verify what tasks are running
    print_tasks();

    // Suspend current task
    vTaskSuspend(NULL);
    
}

/* for debugging see what tasks are doing */
void print_tasks()
{   
    UBaseType_t num_tasks;
    char *task_stats;

    num_tasks =  uxTaskGetNumberOfTasks();
    printf("# of tasks %d\n", (int) num_tasks);
 /* can't put this buffer  on stack, causes overflow, use heap */
    task_stats = (char *) malloc(num_tasks*64); // allow 64 char per task?
    // Check if the memory has been successfully  allocated by malloc or not 
    if (task_stats == NULL)
    { 
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

