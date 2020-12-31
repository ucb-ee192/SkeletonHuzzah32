/* common parameters to only include once */

#define TIMER_DIVIDER         16  //  Hardware timer clock divider on 80 MHz clock
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds