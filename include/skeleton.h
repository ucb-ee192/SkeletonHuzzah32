/* common parameters to only include once */

#define TIMER_DIVIDER         16  //  Hardware timer clock divider on 80 MHz clock
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds

#define MAX_LOG_LENGTH 128       // max # of characters in a line

// errors to be handled. Convention is to have single bit set for OR'ing
#define ERR_UDP 0x01        //problem with UDP connection
#define ERR_MEM 0x02        // problem with memory
#define ERR_TASK_START 0x04 // problem starting a task

/************** prototypes ********************* */
void car_error_handle(unsigned int);

/************** end prototypes ******************/


// structure for commands coming from UDP interface
struct cmd_struct_def {
    char cmd[8];
    int value;
};  