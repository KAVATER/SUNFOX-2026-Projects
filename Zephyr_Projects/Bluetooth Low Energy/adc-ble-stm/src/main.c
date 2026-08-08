/* Include */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
/* ================================== */

/*Definations */
#define ADC_NODE DT_NODELABEL(adc1)
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME)-1) //-1 inorder to remove null character

//Defining service/characteristic UUIDs
#define BT_UUID_ADC_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
#define BT_UUID_ADC_CHRC_VAL \
    BT_UUID_128_ENCODE(0x00001524, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
/*====================================== */

/* Declarations  */
const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);

//static int16_t adc_buffer;

int8_t adc_buffer[14];
uint32_t data_count = 0;

static struct k_work adv_work;

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
    .buffer = &adc_buffer[4],
    .buffer_size = sizeof(adc_buffer)-4,
    .resolution = 12,
};

static const struct bt_data ad[] = {

    /*Advertising flags */
    /*"This device is generally discoverable and can be shown to users for connection." */
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),

    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME,DEVICE_NAME_LEN),
};


/* Scan response packet */
//the following code is advertising a random uuid insted of utilising the service uuid
// static const struct bt_data sd[] =
//     {
//         BT_DATA_BYTES(BT_DATA_UUID128_ALL,
//             BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)),
//     };

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_ADC_SERVICE_VAL),
};

static const struct bt_le_adv_param *adv_param =
        /* BT_LE_ADV_OPT_CONN - "I want to connect my ble devie" */
        /* BT_LE_ADV_OPT_USE_IDENTITY - ""Don't use a temporary/random address.
         *
         Use my permanent Bluetooth identity."" */
        BT_LE_ADV_PARAM(
            (BT_LE_ADV_OPT_CONN|BT_LE_ADV_OPT_USE_IDENTITY),
            800,// Min Advertising Interval 500ms (800*0.625ms)
            801, /*Max Advertising Interval 500.625ms (801*0.625ms)*/
            NULL
        );
//defining service and characteristic UUIDs
static struct bt_uuid_128 adc_service_uuid = BT_UUID_INIT_128(BT_UUID_ADC_SERVICE_VAL);
static struct bt_uuid_128 adc_chrc_uuid    = BT_UUID_INIT_128(BT_UUID_ADC_CHRC_VAL);

//checking if the client has asked for the information
static bool adc_notify_enabled; // flag not needed

static void adc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    adc_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    printk("ADC notifications %s\n", adc_notify_enabled ? "enabled" : "disabled");
}

/* ========================================================================= */

/*Function Definations */
static void adv_work_handler (struct k_work *work)
{
    int err = bt_le_adv_start (adv_param, ad, ARRAY_SIZE(ad),sd, ARRAY_SIZE(sd));

    if(err)
    {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }
    printk("Advertising successfully started\n");
}

static void advertising_start (void)
{
    k_work_submit(&adv_work);
}

static void recycled_cb(void)
{
    printk("Connection object available from previous conn.Disconnect is complete!\n");
    advertising_start();
}

//for multi device connection :
static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    if(err)
    {
        printk("Connection failed (err %u\n",err);
        return;
    }
    printk("Connected\n");
    advertising_start();
}

/* ====================================================================================== */

/* Call Back Function */
BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected_cb,
	.recycled = recycled_cb,
};

// GATT service define :
BT_GATT_SERVICE_DEFINE(adc_svc,
    BT_GATT_PRIMARY_SERVICE(&adc_service_uuid),
    BT_GATT_CHARACTERISTIC(&adc_chrc_uuid.uuid,
        BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_NONE,
       NULL, NULL, NULL ),
    //ccc - client characteristic configuration
    BT_GATT_CCC(adc_ccc_cfg_changed,
                    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

);
/* ==================================================================== */

int main(void)
{
    /*Variable Definations */
    int ret;
    int err;
    bt_addr_le_t addr;
    err = bt_enable(NULL);
    /*=========================== */

   /* If Statements */
    if(!device_is_ready(adc_dev))
    {
        printk("ADC device is not ready!");
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

    // BLE if statements
    if(err)
    {
        printk("Bluetooth intit failed (err%d)\n",err);
        return -1;
    }
    err = bt_addr_le_from_str("FF:EE:DD:CC:BB:AA",
        "random",&addr);

    if(err)
    {
        printk("Invalid BT address (err %d)\n", err);
    }
    err = bt_id_create(&addr,NULL);
    if(err<0)
    {
        printk("Creating new ID failed (err %d)\n ",err);

    }
	k_work_init(&adv_work, adv_work_handler);

	advertising_start();
	/* =============================================================== */

    while(1)
    {
        ret = adc_read(adc_dev, &sequence);

        memcpy(adc_buffer, &data_count, sizeof(data_count));
        data_count++;

        if (ret == 0)
               {
                   printk("ADC Value = ");
                   for (int i = 0; i < sizeof(adc_buffer); i++) {
                       printk("%02x",(uint8_t) adc_buffer[i]);
                   }
                   printk("\n");

                   // printk("ADC Value = %hhn\n", adc_buffer);
       // if(adc_notify_enabled)
            // {
            /* if statement not needed as when you pass NULL for the connection, the
            *function loops over every currently connected device, and for each one individually,
            *it looks up that specific device's CCC value for that specific attribute.
           * Only if that device's CCC says "notifications enabled" does it actually send data to it. */
            bt_gatt_notify(NULL, &adc_svc.attrs[2], adc_buffer, sizeof(adc_buffer));

            // }
               }
               else
               {
                   printk("ADC read failed (%d)\n", ret);
               }

               k_sleep(K_MSEC(10));
    }
    return 0;
}
