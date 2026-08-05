#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/* Advertising packet */
static const struct bt_data ad[] = {

	/* Advertising flags */
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),

	/* Complete device name */
	BT_DATA(BT_DATA_NAME_COMPLETE,
		DEVICE_NAME,
		DEVICE_NAME_LEN),
};

/* URI data:
 * 0x17 -> "https:"
 * Result = https://Hi!
 */
// static unsigned char url_data[] = {
// 	0x17,
//         '/',
//         '/',
// 	'H',
// 	'i',
// 	'!'
// };
static unsigned char url_data[] = { 0x17, '/', '/', 'a', 'c', 'a', 'd', 'e', 'm', 'y', '.',
                                    'n', 'o', 'r', 'd', 'i', 'c', 's', 'e', 'm', 'i', '.',
                                    'c', 'o', 'm' };

/* Scan response packet */
static const struct bt_data sd[] = {

	BT_DATA(BT_DATA_URI,
		url_data,
		sizeof(url_data)),
};

// /* Scannable, non-connectable advertising parameters */
static const struct bt_le_adv_param *adv_param =
	BT_LE_ADV_PARAM(
		BT_LE_ADV_OPT_SCANNABLE,
		800,
		801,
		NULL);

int main(void)
{
	int err;

	/* Enable Bluetooth */
	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return -1;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */
	err = bt_le_adv_start(adv_param,
			      ad,
			      ARRAY_SIZE(ad),
			      sd,
			      ARRAY_SIZE(sd));

	if (err) {
		printk("Advertising failed (err %d)\n", err);
		return -1;
	}
//         err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
//      if (err) {
// 	printk("Advertising failed to start (err %d)\n", err);
// 	return -1;
// }

	printk("Advertising started\n");

	while (1)
	{
    k_sleep(K_SECONDS(1));
	}
}
