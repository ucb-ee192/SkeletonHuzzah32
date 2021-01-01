/* user tasks. Things which block can go here, as this task is low priority 
* and not part of task watchdog.
* Blocking operations such as getchar can not be in main() as they will cause watchdog timeout 
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define BLINK_GPIO 13 // IO13

/* prototypes */
static void userTask(void *pvParameters);
void start_user(void);
extern void print_tasks(void);

void start_user()
{     // TaskFunction_t pvTaskCode, const char * const pcName,  configSTACK_DEPTH_TYPE usStackDepth,
    //  void *pvParameters, UBaseType_t uxPriority,  TaskHandle_t *pxCreatedTask (optional)
    if(xTaskCreate(userTask, "usertask", 2048, NULL, tskIDLE_PRIORITY, NULL) !=pdPASS)
    {   printf("User Task creation failed! reboot needed.\r\n");
        while (1); // hang indefinitely
    }
    
}

/* just have a task execute continually blinking LED. This should keep task watch dog timer restting */

static void userTask(void *pvParameters)
{   double z=3.14159;
    uint8_t ch;
    printf("User Task started\n");  
    printf("Check floating point 3.14159=%10.3f\n", z);

    printf("Echoing character input:");
    printf("r for reset CPU   x for dump core \t t for tasks \n");
    while(1) 
    {   ch = fgetc(stdin);
	    if (ch!=0xFF)
	    {
		    fputc(ch, stdout);
	        vTaskDelay(100/ portTICK_PERIOD_MS); // allow IDLE process to run, else get watchdog timer
            switch(ch){
                case 'x':
                    printf("About to dump core\n");
                    fflush(stdout);
                    assert(0); // trigger core dump
                    break;
                case 'r':
                    for (int i = 10; i >= 0; i--) 
                    {
                        printf("Restarting in %d seconds...\n", i);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                    }       
                    printf("Restarting now.\n");
                    fflush(stdout);
                    esp_restart();
                    break;
                case 't':
                    print_tasks();
                    break;
                default:
                    printf("r for reset CPU   x for dump core \t t for tasks \n");
                    break;
            }
        }   
    }
}

