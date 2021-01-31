/* User Tasks Script
EE 192, Spring 2021, R. Fearing

ENTER DESCRIPTION

User tasks. Things which block can go here, as this task is low priority and not part of task watchdog.
Blocking operations such as getchar can not be in main() as they will cause watchdog timeout 

*/

// Includes
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "sdkconfig.h"
#include "skeleton.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
extern xQueueHandle cmd_queue;          // this is set by log_init 

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void userTask(void *pvParameters);
void start_user(void);
extern void print_tasks(void);
extern void log_add(char *);
void print_help(void);
void get_time(void);
void uart_cmd(uint8_t);
void udp_cmd(char *, int);

/*******************************************************************************
 * Functions
 ******************************************************************************/

// Start user task
void start_user()
{     // TaskFunction_t pvTaskCode, const char * const pcName,  configSTACK_DEPTH_TYPE usStackDepth,
    //  void *pvParameters, UBaseType_t uxPriority,  TaskHandle_t *pxCreatedTask (optional)
    if(xTaskCreate(userTask, "usertask", 2048, NULL, tskIDLE_PRIORITY, NULL) !=pdPASS)
    {   printf("User Task creation failed! reboot needed.\r\n");
        car_error_handle(ERR_TASK_START);
    }  
}

// User task for interface and debugging
static void userTask(void *pvParameters)
{   uint8_t ch;
    int cmdready;
    struct cmd_struct_def cmd_struct;       // define structure to receive commands

    printf("User Task started\n");  
    print_help();
    while(1) 
    {   vTaskDelay(100/ portTICK_PERIOD_MS); // allow IDLE process to run, else get watchdog timer
        // check for keyboard input
        ch = fgetc(stdin);
	    if (ch!=0xFF)
        { uart_cmd(ch);}

        // check for command waiting in queue
        cmdready =  uxQueueMessagesWaiting(cmd_queue);
        if (cmdready >= 1)  // get item from cmd queue
        {   if(xQueueReceive(cmd_queue, &cmd_struct, portMAX_DELAY) != pdPASS)
            { printf("error reading from cmd_queue\n"); }
            udp_cmd(cmd_struct.cmd, cmd_struct.value);
        }
    }
}

// handle commands from UDP, format command (8 characters), value (int)
void udp_cmd(char command[], int value)
{ char ucommand[8];
    int i;
    for(i =0; i< sizeof(ucommand); i++)
        ucommand[i] = (char) toupper((int) command[i] ); // convert to upper case
    
    printf("\n In udp_command. command:%s value %d\n", ucommand, value);
    if (strcmp(ucommand,"TIME") == 0) 
        get_time();
}


// handle UART commands in case UART is plugged in
void uart_cmd(uint8_t ch)
{   fputc(ch, stdout);
	switch(ch)
    {   case 'x':
            printf("About to dump core\n");
            fflush(stdout);
            *((int *) 0) = 0; // cause panic by writing to nonexistent address 
            // trigger core dump
            break;
        case 'r':
            for (int i = 10; i >= 0; i--) 
            {   printf("Restarting in %d seconds...\n", i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            }       
            printf("Restarting now.\n");
            fflush(stdout);
            esp_restart();
            break;
        case 't':
            print_tasks();
            break;
        case 'z':
            get_time();
            break;
        case '?':
        case 'h':
        default:
            print_help();
            break;
    }
}


void print_help()
{  printf("\n r for reset CPU  \t x for dump core \t t for tasks \t h for help  \t  z for time \n");
   printf("Enter for command:");
}

void get_time()
{   char log[MAX_LOG_LENGTH + 1];
    uint64_t task_counter_value;
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);
    snprintf(log, sizeof(log),"Current time is %8.3f sec \n", (double) task_counter_value / TIMER_SCALE);
    log_add(log);
}
 

 /*  stub error handler, add code to this function!
 * should stop car, maybe reset CPU, etc?
 * should blink LED or drive buzzer to alert error, since UART is not connected! 
*/
 void car_error_handle(unsigned int errnum)
 {  switch(errnum)
    {
        case ERR_UDP:
            printf("\n Error connecting UDP. Halting.\n");
            break;
        default:
            printf("\n car_error_handle. Error %x. Halting.\n", errnum);
            break;
    }
    while(1); // halt here, could also do cpu restart, e.g. esp_restart();
 }