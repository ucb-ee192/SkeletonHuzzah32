/* Wifi Logging Script
EE 192, Spring 2021, R. Fearing

ENTER DESCRIPTION

Relevant license and usage information from original source:
https://github.com/nkolban/esp32-snippets/blob/bf8649ba5df3b154866a814014fd97c027e76263/wifi/fragments/access-point.c
https://github.com/nkolban/esp32-snippets/blob/master/wifi/fragments/access-point.c
downloaded from https://gist.github.com/narfdotpl/329c201534903396e5851b38bf715ce9

*/ 

// Includes
#include <esp_event.h>
//#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <lwip/sockets.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>
#include "driver/ledc.h"
#include "skeleton.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LINELEN 128                     // Line length for logging message

static const char *TAG = "example";     // for logging
static int sock;                        // needs to be available  to be passed to wifi log
const int port = 5555;
extern xQueueHandle log_queue;          // this is set by log_init in uart-logtask.c
extern xQueueHandle cmd_queue;          // this is set by log_init 
struct cmd_struct_def cmd_struct;       // define structure to receive commands

// Capture address from each distinct address
struct sockaddr_in source_addr; // Large enough for both IPv4 or IPv6
socklen_t socklen = sizeof(source_addr);

/*******************************************************************************
 * Prototypes
 ************** ****************************************************************/
void connect_client(int);


/*******************************************************************************
 * Functions
 ******************************************************************************/

// Returns success of wifi event
esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
    return ESP_OK;
}

// Starts a wifi access point
void wifi_start_access_point() 
{  // Define wifi configuration to be referenced in wifi setup routine
    wifi_config_t wifi_config = {
        .ap = {
            .ssid="Huzzah32",
            .ssid_len=0,
            .password="",
            .channel=6,
            .authmode=WIFI_AUTH_OPEN,
            .ssid_hidden=0,
            .max_connection=4,
            .beacon_interval=100
        }
    };

    // Using legacy API (expect deprecated warnings on compile)
    tcpip_adapter_init();
    esp_event_loop_init(wifi_event_handler, NULL);
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();
}



void connect_client(int sock) 
{   char *text_message ="Connected to Huzzah32!";   // Message upon connecting successfully
    int size_read;
    
    // Attempt to receive from connecting address
    printf("Waiting to receive first message\n");
    size_read = recvfrom(sock, &cmd_struct, sizeof(cmd_struct), 0, (struct sockaddr *)&source_addr, &socklen);
    if (size_read <= 0) 
    {  car_error_handle(ERR_UDP); }
   
    printf("UDP connection established.\n %d bytes from %s \n",
        size_read, inet_ntoa(source_addr.sin_addr.s_addr));
    printf("commmand: %s  value = %d \n", cmd_struct.cmd, cmd_struct.value);
    xQueueOverwrite(cmd_queue, &cmd_struct);  // put most recent command on queue, over writing any previous cmd
 
  // Send message on connect
    int err = sendto(sock, text_message, strlen(text_message), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
    if (err < 0)
    {   ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        car_error_handle(ERR_UDP);
    }
}

// Start the socket server hosted on the ESP32 that will receive and reply to connecting machine
int start_socket_server() {
    // Create and listen on the socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port)
    };
    bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    connect_client(sock);  // Use reply address to send log data to
       
    return(sock);
}

// Wifi log task that will run in the background and send info to connected machine
static void wifi_log_task(void *pvParameters)
{   uint32_t scount = 0; // count sent log messages
    uint32_t rcount = 0; // count received messages
    char log[LINELEN+1];
    char linebuffer[LINELEN+16+1];
    struct pollfd poll_set[1]; //initialize file descriptor for socket, here only need 1
    UBaseType_t logready;  // >=1 for log entires in queue                       
    int poll_val, size_read;
   
    printf("wifi_task using socket %d\n", sock);  // global static
    fflush(stdout);
   
    // initialize structure for polling socket
    poll_set[0].fd = sock;
    poll_set[0].events = POLLIN | POLLRDNORM | POLLRDBAND;
    poll_set[0].revents = 0;
    
    while(1)
    {   logready =  uxQueueMessagesWaiting(log_queue);
        if (logready >= 1)  // send item from log queue 
        {   if(xQueueReceive(log_queue, log, portMAX_DELAY) != pdPASS)
            { printf("error reading from log_queue\n"); }
            // have not timed this printf- may be slow and use lots of stack, may be better to use atoi(), etc.
            snprintf(linebuffer, sizeof(linebuffer),"Log %d: %s",scount,log);
            // CAUTION: sendto can block if sent too much data. May want to use non blocking
            int err = sendto(sock, linebuffer, strlen(linebuffer), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
            if (err < 0)
            {   ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                while(1); // halt 
            }
            scount++;
        }
        /* now check for input from socket */
        // int poll(struct pollfd fds[], nfds_t nfds, int timeout); -- timeout in milliseconds, 0 return immediately
        poll_val= poll(poll_set, (nfds_t)1, 0);  // returns <0 for error, 0 for timed out, =1 for data
        if (poll_val < 0)
        {  printf("poll error, errno=%d\n", errno); }
        else if (poll_val == 1) // 1 file descriptor ready to read
        {   // printf("poll %d fd ready. revents = %x\n", poll_val, poll_set[0].revents);
            //size_read = recvfrom(sock, linebuffer, sizeof(linebuffer), 0, (struct sockaddr *)&source_addr, &socklen);
            size_read = recvfrom(sock, &cmd_struct, sizeof(cmd_struct), 0, (struct sockaddr *)&source_addr, &socklen);
            if (size_read <= 0) 
            {   printf("socket read error\n");     }
            printf("%d bytes from %s ", size_read, inet_ntoa(source_addr.sin_addr.s_addr)); 
            printf("commmand: %s  value = %d \n", cmd_struct.cmd, cmd_struct.value);
            xQueueOverwrite(cmd_queue, &cmd_struct);  // put most recent command on queue, over writing any previous cmd
            rcount++;  // received message counter
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);   
    }
}

// Begin logging over wifi connection
void wifi_log_start()
{    // TaskFunction_t pvTaskCode, const char * const pcName,  configSTACK_DEPTH_TYPE usStackDepth,
    //  void *pvParameters, UBaseType_t uxPriority,  TaskHandle_t *pxCreatedTask (optional)
    if (xTaskCreate(wifi_log_task, "wifi_log_task", configMINIMAL_STACK_SIZE + 2048, &sock, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {   printf("Wifi Log Task creation failed!. Reset needed.\r\n");
        while (1);
    }
}

// Organize startup tasks for initiating wifi logging
void wifi_start() 
{   // Needed for code stored in flash
    nvs_flash_init();
    printf("\n*** starting access point ***\n");
    fflush(stdout);                                     // Flush stdout so wifi startup not delayed
    wifi_start_access_point();                          // Create access point on ESP32
    printf("\n*** starting socket server ***\n");
    fflush(stdout);
    sock=start_socket_server();                         // Start socket server, wait for reply, and store reply address
    printf("Using socket %d\n", sock);
    printf("\n *** Starting wifi task send to client ***\n");
    fflush(stdout);
    wifi_log_start();                                   // Begin logging over established wifi connection
}