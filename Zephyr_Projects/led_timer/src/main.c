#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define TIMER_MS 500
#define LED_NODE DT_ALIAS(led0)

static struct k_timer led_timer; 
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE,gpios);

void timer_callback(struct k_timer *timer)
{
	if(timer == &led_timer)
	{
	 gpio_pin_toggle_dt(&led);
	}
}

int main (void)
{

  gpio_pin_configure_dt(&led,GPIO_OUTPUT);

  k_timer_init(&led_timer, timer_callback, 	NULL);
  
  k_timer_start (&led_timer, K_MSEC(TIMER_MS), K_MSEC(TIMER_MS));

  while(1)
  {
	k_sleep(K_FOREVER);
  }
return 0;
}