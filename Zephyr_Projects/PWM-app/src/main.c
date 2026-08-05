#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#define PWM_PERIOD_NS 20000000
#define PWM_PULSE_WIDTH 1400000

#define PWM_PERIOD_NS 100000000
#define PWM_PULSE_WIDTH 14000000
/* Gpio node */
// #define GPIO0_NODE DT_NODELABLE(gpio0)
// static const struct device *gpio_dev = DEVICE_DT_GET(GPIO0_NODE);

// #define LED_NODE DT_ALIAS(led0)

// staic const struct pwm_dt_spec led_pwm = PWM_DT_GET_SPEC(DT_NODELABEL(my_node));

#define PWM_LED_NODE DT_ALIAS(pwm_led0)
static const struct pwm_dt_spec led_pwm = PWM_DT_SPEC_GET(PWM_LED_NODE);

// int set_led_brightness(uint8_t percent)
// {
//     if(!device_is_ready(led_pwm.dev))
//     {
//         return -ENODEV;// ENODEV = 19 , NO SUCH DEVICE
//     }
//     uint32_t pulse = led_pwm.period / 100 * percent;

//     return pwm_set_dt(&led_pwm, led_pwm.period, pulse);
// }

int main()
{
    if(!pwm_is_ready_dt(&led_pwm))
    {
        printk("Error\n",led_pwm->name);
        return 0;
    }

    if(err)
    {
        printk("Getting error = %d/n",err);
        return 0;
    }

    


    
}


