#include <stdio.h>
#include <zephyr/kernel.h>

#define TIMER_MS 1000

//struct for holding timer information
static struct k_timer my_timer;

//timer clalback
void timer_callback(struct k_timer *timer)
{
	if(timer == &my_timer)
	{
		printk("Timer !!\r\n");
	}
}
int main(void)
{
	k_timer_init(&my_timer, timer_callback, NULL);

	k_timer_start(&my_timer, K_MSEC(TIMER_MS), K_MSEC(TIMER_MS));

	while(1)
	{
		k_sleep(K_FOREVER);
	}
	return 0;
}