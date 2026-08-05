#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED_NODE DT_ALIAS(led0)

#define Timer_led_on 200
#define Timer_led_off 800

static struct k_timer led_timer ;
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE,gpios);

static bool led_state = false;

void timer_callback(struct k_timer *timer)
{
	if(timer == &led_timer)
	{
		 gpio_pin_toggle_dt(&led);
		
		
		if(led_state)
		{
           k_timer_start(&led_timer, K_MSEC(Timer_led_on), K_MSEC(Timer_led_on));
		}
     else
	 {
	 k_timer_start(&led_timer, K_MSEC(Timer_led_off), K_MSEC(Timer_led_off));
	 }
	 led_state = !led_state;
	}
}

int main (void)
{
	gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	k_timer_init(&led_timer, timer_callback, NULL);
	k_timer_start(&led_timer,K_MSEC(Timer_led_on), K_NO_WAIT);

	while(1)
	{
		k_sleep(K_FOREVER);
	}
	return 0;
}