#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME)-1) //-1 inorder to remove null character

static struct k_work adv_work;

static const struct bt_data ad[] = {

    /*Advertising flags */
    /*"This device is generally discoverable and can be shown to users for connection." */
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),

    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME,DEVICE_NAME_LEN),
};


/* Scan response packet */
static const struct bt_data sd[] = 
    {
        BT_DATA_BYTES(BT_DATA_UUID128_ALL,
            BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)),
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

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.recycled = recycled_cb,
};

int main(void)
{
    int err;
    bt_addr_le_t addr;

    err = bt_enable(NULL);
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
	k_sleep(K_FOREVER); //sleeping the cpu
        
}


