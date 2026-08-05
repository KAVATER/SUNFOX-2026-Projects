
// code to blink led using timer
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE,gpios);

//===== timer call back function =====

void timer_handler(struct k_timer *timer)
{
    static bool state = false;
    state = !state;
    gpio_pin_set_dt(&led, state);
}

K_TIMER_DEFINE(my_timer, timer_handler, NULL);

int main(void)
{
    if (!gpio_is_ready_dt(&led))
    {
        return 0;
    }
    gpio_pin_configure_dt(&led, GPIO_OUTPUT);

    gpio_pin_set_dt(&led, 0);

    k_timer_start(&my_timer, K_SECONDS(1), K_SECONDS(1));

    while (1)
    {
        k_sleep(K_FOREVER);
    }
}
