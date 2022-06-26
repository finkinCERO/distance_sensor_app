#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

#define SLEEP_TIME_MS 300

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define IN0_NODE DT_ALIAS(in0)
#if !DT_NODE_HAS_STATUS(IN0_NODE, okay)
#error "Unsupported board: in0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec sensor = GPIO_DT_SPEC_GET_OR(IN0_NODE, gpios, { 0 });
static struct gpio_callback sensor_cb_data;

#define IN1_NODE DT_ALIAS(in1)
#if !DT_NODE_HAS_STATUS(IN1_NODE, okay)
#error "Unsupported board: in0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec sensor2 = GPIO_DT_SPEC_GET_OR(IN1_NODE, gpios, { 0 });
static struct gpio_callback sensor2_cb_data;

/*
 * The led0 devicetree alias is optional. If present, we'll use it
 * to turn on the LED whenever the button is pressed.
 */
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, { 0 });

int parkplatz_a = 1;
int parkplatz_b = 1;

void sensor_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int val = gpio_pin_get_dt(&sensor);
	int val2 = gpio_pin_get_dt(&sensor2);
	if (val == 1) {
		parkplatz_a = 1;
	}
	else if (val == 0)
	{
		parkplatz_a = 0;
	}
	if (val2 == 0) {
		parkplatz_b = 1;
	}
	else if (val2 == 1)
	{
		parkplatz_b = 0;
	}
	int val3 = 2 - parkplatz_a - parkplatz_b;
	//printk("available parking spots: %d\n", val3);
	/* printk("Sensor called back at %" PRIu32 "\n", k_cycle_get_32()); */
}
void sensor_callback3()
{
	int val = gpio_pin_get_dt(&sensor);
	int val2 = gpio_pin_get_dt(&sensor2);
	//printk("# %d\n",val);
	//printk("* %d\n",val2);
	if (val == 1) {
		parkplatz_a = 1;
	}
	else if (val == 0)
	{
		parkplatz_a = 0;
	}
	if (val2 == 0) {
		parkplatz_b = 1;
	}
	else if (val2 == 1)
	{
		parkplatz_b = 0;
	}
	int val3 = 2 - parkplatz_a - parkplatz_b;
	printk("available parking spots: %d\n", val3);
	/* printk("Sensor called back at %" PRIu32 "\n", k_cycle_get_32()); */
}
void sensor2_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	int val = gpio_pin_get_dt(&sensor2);

	if (val == 1 && parkplatz_b > 0) {
		parkplatz_b = parkplatz_b - 1;
	}
	else if (val == 0 && parkplatz_b < 1)
	{
		parkplatz_b = parkplatz_b + 1;
	}
	//printk("[SENSOR 2]\tavailable parking spots: %d\n", parkplatz_b);
	/* printk("Sensor called back at %" PRIu32 "\n", k_cycle_get_32()); */
}

void main(void)
{
	int ret;
	if (!device_is_ready(sensor.port)) {
		printk("Error: sensor 1 device %s is not ready\n", sensor.port->name);
		return;
	}
	if (!device_is_ready(sensor2.port)) {
		printk("Error: sensor 2 device %s is not ready\n", sensor2.port->name);
		return;
	}
	ret = gpio_pin_configure_dt(&sensor, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n", ret, sensor.port->name,
		       sensor.pin);
		return;
	}
	ret = gpio_pin_configure_dt(&sensor2, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n", ret, sensor2.port->name,
		       sensor2.pin);
		return;
	}
	ret = gpio_pin_interrupt_configure_dt(&sensor, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n", ret,
		       sensor.port->name, sensor.pin);
		return;
	}
	ret = gpio_pin_interrupt_configure_dt(&sensor2, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n", ret,
		       sensor2.port->name, sensor2.pin);
		return;
	}
	gpio_init_callback(&sensor_cb_data, sensor_callback, BIT(sensor.pin));
	int cb = gpio_add_callback(sensor.port, &sensor_cb_data);
	if (cb != 0) {
		printk("Error %d: failed to add callback on %s pin %d\n", cb, sensor.port->name,
		       sensor.pin);
		return;
	}
	gpio_init_callback(&sensor2_cb_data, sensor2_callback, BIT(sensor2.pin));
	int cb2 = gpio_add_callback(sensor2.port, &sensor_cb_data);
	if (cb2 != 0) {
		printk("Error %d: failed to add callback on %s pin %d\n", cb, sensor.port->name,
		       sensor.pin);
		return;
	}

	printk("Set up sensor at %s pin %d\n", sensor.port->name, sensor.pin);
	printk("Set up sensor at %s pin %d\n", sensor2.port->name, sensor2.pin);
	if (led.port && !device_is_ready(led.port)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n", ret, led.port->name);
		led.port = NULL;
	}
	if (led.port) {
		ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n", ret,
			       led.port->name, led.pin);
			led.port = NULL;
		} else {
			printk("Set up LED at %s pin %d\n", led.port->name, led.pin);
		}
	}
	printk("Waiting for sensor output\n");
	while (1) {
		sensor_callback3();
		k_msleep(500);
	}
}
