// /*
//  * Copyright (c) 2021 Nordic Semiconductor ASA
//  * SPDX-License-Identifier: Apache-2.0
//  */

#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/drivers/gpio.h>

#define BUTTON_NODE DT_ALIAS(sw0)
#define LED_NODE DT_ALIAS(led0)

//struct for extracting all the information about the button
//static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

//struct to store all the led information
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

//============ code for interupt =================
static void button_input (struct input_event *evt, void *user_data)
{
	if (evt->sync ==0)
	{
		return;
	}
    
	//======= the folowing code is not needed as it has been read already in input subsystem
//	uint8_t button_state = gpio_pin
	// if(button_state == 0)
	// {
	// 	gpio_pin_set_dt(&led,1);
	// }
	//else gpio_pin_set_dt(&led,0);
	if (evt->value)
{
    gpio_pin_set_dt(&led, 1);
}
else
{
    gpio_pin_set_dt(&led, 0);
}
printk("sync = %d\n", evt->sync);
printk("value = %d\n", evt->value);
}

INPUT_CALLBACK_DEFINE(NULL, button_input, NULL);

int main(void)
{
	  if (!gpio_is_ready_dt(&led))
    {
        return 0;
    }
	 gpio_pin_configure_dt(&led, GPIO_OUTPUT);
    
	 gpio_pin_set_dt(&led, 0);
	k_sleep(K_FOREVER);

	return 0;
}



//code for polling 
// int main(void)
// {
//         if(!gpio_is_ready_dt(&button) || !gpio_is_ready_dt(&led))
//         {
//         return 0;
//         }

//         gpio_pin_configure_dt(&button, GPIO_INPUT);
//         gpio_pin_configure_dt(&led, GPIO_OUTPUT);

//         while(1)
//         {
//                 uint8_t button_state = gpio_pin_get_dt(&button);

//                 if(button_state == 0 )
//                 {
//                         gpio_pin_set_dt(&led,0);
//                 }
//                 else gpio_pin_set_dt(&led,1);

//                 k_msleep(10);
//         }
// }


