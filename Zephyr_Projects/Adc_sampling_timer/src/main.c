#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>

#define LED_NODE DT_ALIAS(led0)
#define timer_interval 100
#define ADC_NODE DT_NODELABEL(adc)

static int16_t adc_buffer;


static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);
static struct k_timer my_timer;
//static struct adc_sequence sequence;

//configuring adc channel
static struct adc_channel_cfg channel_cfg = {
    .gain = ADC_GAIN_1,
    .reference = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id = 0,
};


struct adc_sequence sequence = {
    .channels    = BIT(0),
    .buffer      = &adc_buffer,
    .buffer_size = sizeof(adc_buffer),
    .resolution  = 12,
};

void timer_callback(struct k_timer *timer)
{
if(timer == &my_timer)
{
     adc_read(adc_dev,&sequence);
     printk("Adc value at every 100ms is : %d\n",adc_dev);
}
}


int main(void)
{
 gpio_pin_configure_dt(&led, GPIO_OUTPUT);
 k_timer_init(&my_timer,timer_callback,NULL);

 adc_channel_setup(adc_dev, &channel_cfg);//configuring adc channel

 k_timer_start(&my_timer,K_MSEC(timer_interval),K_MSEC(timer_interval));

while(1)
{
    k_sleep(K_FOREVER);
}
}