#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/of_device.h>
#include <asm/io.h>

// Module Name
#define MODULE_NAME "snakePlayer"

// GPIO sizes
#define BTN_GPIO_COUNT 4

#define MEM_REG_SIZE 0x100000

#define GPIO_DATA 0x00
#define GPIO_TRI 0x04
#define GPIO2_DATA 0x08
#define GPIO2_TRI 0x0C
#define GIER_REG 0x011C
#define IER_REG 0x0128
#define ISR_REG 0x0120

// variables used in this module
static int irqnum; // SW GPIO IRQ Number
static void __iomem *btn_regs; // BTN GPIO registers
static unsigned int btn_gpio_base = 901;

// get current switch states
static unsigned int get_button_states(void)
{
  unsigned int state = 0, i;

  //TODO: read current GPIO state and do bitwise operations on
  //      state variable
  for (i = 0; i < BTN_GPIO_COUNT; ++i) {
    state |= gpio_get_value(btn_gpio_base + i) << i;
  }

  // return the sate of all switches
  return state;
}

/* @brief Initialize GPIOs */
static void initialize_gpio(void)
{
    unsigned int i;

    for (i = 0; i < BTN_GPIO_COUNT; ++i) {
        gpio_direction_input(btn_gpio_base + i);
    }
}

/* interrupt handler */
static irqreturn_t btn_irq_handler(int irq, void* dev_id)
{
    unsigned int states;

    // Debugging message
    printk(KERN_INFO "Button IRQ handler\n");

    states = get_button_states();


    // WRITE TO OLED DRIVER INFO HERE


    // Clear ISR flag
    // Assuming the specific register to clear the interrupt flag is named INT_FLAG_REG
    iowrite32(0x01, btn_regs + ISR_REG);

    return IRQ_HANDLED;
}

static void enable_interrupts(void)
{
    // Enable interrupts in the AXI GPIO controller
    // Set tri buffer to 1
    iowrite32(0x01, btn_regs + GPIO_TRI);
    printk(KERN_INFO "GPIO_TRI: %x\n", ioread32(btn_regs + GPIO_TRI));

    // enable global interrupt in the GIER register, set bit 31
    iowrite32(0x80000000, btn_regs + GIER_REG);
    printk(KERN_INFO "GIER: %x\n", ioread32(btn_regs + GIER_REG));

    // 1. enable all IER bits
    iowrite32(0x3, btn_regs + IER_REG);
    printk(KERN_INFO "IER: %x\n", ioread32(btn_regs + IER_REG));

    // // 2. enable bit 0 in the MER register (ME bit), 
    // // HIE is bit 1("If software testing of hardware interrupts is to be performed, the HIE bit must remain at its reset value of 0.")
    // // writel(0x03, sw_regs + MER_REG);
    // iowrite32(0x01, sw_regs + MER_REG);
}

// initialize module
static int mymod_init(void)
{
    int err;
    struct gpio_chip* chip;
    struct platform_device* pdev;
    struct device* dev;
    struct gpio_desc* gdesc;
    struct resource *res;

    // printk(KERN_INFO "btn_gpio_base = %u\n", btn_gpio_base);

    // find GPIO description by GPIO number passed
    gdesc = gpio_to_desc(btn_gpio_base);
    if (!gdesc)
        {
        printk("error getting GPIO description for buttons!\n");
        return -ENODEV;
        }

    // get chip of switches gpio
    chip = gpiod_to_chip(gdesc);
    if (!chip)
        {
        printk("error getting button GPIO chip\n");
        return -ENODEV;
        }

    // retrieve original platform device from gpio chip,
    // now we can extract information from the device tree node
    dev = chip->parent;
    pdev = to_platform_device(dev);

    // printk(KERN_INFO "pdev name: %s\n", pdev->name);
    // printk(KERN_INFO "pdev id: %d\n", pdev->id);
    // printk(KERN_INFO "pdev num resources: %d\n", pdev->num_resources);

    // Acquire the correct hardware interrupt number
    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (!res) {
        printk(KERN_ERR "Failed to get IRQ resource\n");
        return -ENODEV;
    }
    irqnum = res->start;

    // printk(KERN_INFO "IRQ number: %d\n", irqnum);
    // printk(KERN_INFO "IRQ flags: %lu\n", res->flags);
    // printk(KERN_INFO "IRQ start: %llu\n", (unsigned long long)res->start);
    // printk(KERN_INFO "IRQ end: %llu\n", (unsigned long long)res->end);
    // printk(KERN_INFO "Resource size: %llu\n", (unsigned long long)resource_size(res));
    // printk(KERN_INFO "IRQ name: %s\n", res->name);
    // printk(KERN_INFO "Memory region start address: %pa\n", &res->start);

    // Request an operating system interrupt
    err = request_threaded_irq(irqnum, btn_irq_handler, NULL, IRQF_TRIGGER_RISING, MODULE_NAME, NULL);
    if (err) {
        printk(KERN_ERR "Failed to request IRQ\n");
        return err;
    }

    // Retrieve memory region
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        printk(KERN_ERR "Failed to get IRQ resource\n");
        return -ENODEV;
    }

    // Retrieve memory region and map to btn_regs
    btn_regs = devm_ioremap(dev, res->start, resource_size(res));
    if (IS_ERR(btn_regs))
    {
        return PTR_ERR(btn_regs);
    }

    // Enable interrupts in the AXI GPIO controller
    enable_interrupts();

    // initialize GPIOs
    initialize_gpio();

    printk(KERN_INFO "Snake Player module loaded\n");

    // A non 0 return means init_module failed; module can't be loaded.
    return 0;
}

// remove module and perform cleanup
static void mymod_exit(void)
{
  // Cleanup: release the requested interrupt
  free_irq(irqnum, NULL);

  // Unmap memory region
  iounmap(btn_regs);
}

// declare initialization function
module_init(mymod_init);
module_exit(mymod_exit);

// declare module description and license
MODULE_DESCRIPTION("Snake Game Player Module");
MODULE_LICENSE("GPL");
