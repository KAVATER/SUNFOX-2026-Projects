#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED_NODE DT_ALIAS(led0)
#define timer_interval 1000

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE,gpios);
static struct k_timer my_timer;

uint8_t timerCall_count = 1;

void timer_callback(struct k_timer*timer)
{
  
  if(timer == &my_timer)
  {
	if(timerCall_count <=10)
	{
	gpio_pin_toggle_dt(&led);
	printk("Led Blink NO.%u \n",timerCall_count);
	}
	else
	{
      gpio_pin_toggle_dt(&led);
	  printk("Led Toggle Stop \n");
      k_timer_stop(&my_timer);
	}
   timerCall_count++;
  }

}

int main(void)
{
	gpio_pin_configure_dt(&led,GPIO_OUTPUT);
	k_timer_init(&my_timer, timer_callback, NULL);
	k_timer_start(&my_timer,K_MSEC(timer_interval), K_MSEC(timer_interval));

	while(1)
	{
		k_sleep(K_FOREVER);
	}
}