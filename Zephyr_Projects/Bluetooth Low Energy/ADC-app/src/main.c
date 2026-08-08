#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>

#define ADC_NODE DT_NODELABEL(adc)

const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);

static int16_t adc_buffer;

//configuring adc channel
static struct adc_channel_cfg channel_cfg = {
    .gain = ADC_GAIN_1,
    .reference = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id = 0,
};

// adc sequence configuration
static struct adc_sequence sequence = {
    .channels = BIT(0),
    .buffer = &adc_buffer,
    .buffer_size = sizeof(adc_buffer),
    .resolution = 12,
};

int main(void)
{
    int ret;

    if(!device_is_ready(adc_dev))
    {
        printf("ADC device is not ready!");
        return -1;
    }

    //configure adc channel
    ret = adc_channel_setup(adc_dev, &channel_cfg);

    if(ret<0)
    {
        printk("ADC channel setup failed (%d)\n", ret);
         return -1;
    }
    printk("ADC initialized successfully.\n");

    while(1)
    {
        ret = adc_read(adc_dev, &sequence);
        
        if (ret == 0)
               {
                   printk("ADC Value = %d\n", adc_buffer);
               }
               else
               {
                   printk("ADC read failed (%d)\n", ret);
               }
       
               k_sleep(K_MSEC(500));
    }
    return 0;
}
