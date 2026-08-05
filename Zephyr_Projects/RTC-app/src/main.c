#include "mdk/nrf53/nrf5340/nrf5340_application_peripherals.h"
#include <stdio.h>
#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/kernel_includes.h>
#include <zephyr/kernel_includes.h>
#include <zephyr/drivers/rtc.h>

//============ The following method will not work with nrf as they dont have seprate rtc registers 
// 
// const struct device *rtc;

// int main()
// {
//     // struct rtc_time time;
//      struct rtc_time current_time;
     
//     rtc = DEVICE_DT_GET(DT_NODELABEL(rtc));
    
//     if(!device_is_ready(rtc)) {
// 	return 0;
//     }
    

    
//     struct rtc_time time = {
//         .tm_sec = 0,
//         .tm_min = 0,
//         .tm_hour = 5,
//         .tm_mday = 20,
//         .tm_mon = 7,
//         .tm_year = 2026
//     };
    
//     rtc_set_time(rtc, &time);  

//     while(1)
//     {
//         rtc_get_time(rtc, &current_time);
        
//     printf("Date : %02d/%02d/%04d\n",
//             current_time.tm_mday,
//             current_time.tm_mon,
//             current_time.tm_year);
    
//     printf("Time : %02d:%02d:%02d\n",
//             current_time.tm_hour,
//             current_time.tm_min,
//             current_time.tm_sec);

//     k_sleep(K_SECONDS(1));
//     }
//     return 0;
// }
// ========== Using POSIX timer API =============


timer_t my_timer;

void timer_callback(union sigval val)
{
    printk("RTC Timer expired!\n");
}

int main(void)
{
    struct sigevent sev =
    {
        .sigev_notify = SIGEV_THREAD,
        .sigev_notify_function = timer_callback,
    };

    timer_create(CLOCK_MONOTONIC,
                &sev,
                &my_timer);


    struct itimerspec its =
    {
        .it_value.tv_sec = 1,
        .it_value.tv_nsec = 0,

        .it_interval.tv_sec = 1,
        .it_interval.tv_nsec = 0,
    };

    timer_settime(my_timer,
                  0,
                  &its,
                  NULL);


    while (1)
    {
        k_sleep(K_SECONDS(1));
    }
}