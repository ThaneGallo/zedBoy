/* LAB4 AUDIO WITH FIFO */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <asm/spinlock.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
#include <linux/delay.h>
#include <linux/irqflags.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>


#define DRIVER_NAME "esl-oled"


//https://docs.xilinx.com/r/en-US/pg153-axi-quad-spi/Port-Descriptions
// spi register space
// static int SRR = 0x40;        // software reg reset
// static int SPICR = 0x60;      // spi cont reg
// static int SPISR = 0x64;      // spi stat reg
// static int SPIDTR = 0x68;     // spi data trans reg
// static int SPIDRR = 0x6C;     // spi data rec reg
// static int SPISSR = 0x70;     // slave select
// static int SPI_TXFIFO = 0x78; // occupancy tx
// static int SPI_RXFIFO = 0x74; // occupancy rx

// // interrupts
// static int DGIER = 0x1C; //  dev global ier
// static int IPISR = 0x20; // ip isr
// static int IPIER = 0x28; // ip ier

// button interruptss
// #define BTN_GPIO_ADDR 0x41220000 
static unsigned int BTN_GPIO_ADDR = 0x41220000;
#define BUTTON_GPIO_COUNT 8
static int button_irqnum; // Button GPIO IRQ Number
static void __iomem *button_regs; // BUTTON GPIO registers

#define GPIO_DATA 0x00
#define GPIO_TRI 0x04
#define GPIO2_DATA 0x08
#define GPIO2_TRI 0x0C
#define GIER_REG 0x011C
#define IER_REG 0x0128
#define ISR_REG 0x0120

// structure for an individual instance (ie. one audio driver)
struct esl_oled_instance
{
  void *__iomem regs;  // fifo registers
  struct cdev chr_dev; // character device
  dev_t devno;

  // list head
  struct list_head inst_list;

  // interrupt number
  unsigned int irqnum;

  // oled fifo size
  unsigned int fifo_size;

  // fifo buffer
  unsigned char fifo_buf[31];
};

// matching table
static struct of_device_id esl_oled_of_ids[] = {
    {.compatible = "esl,null-spi"},
    {}};

// structure for class of all oled drivers
struct esl_oled_driver
{
  dev_t first_devno;
  struct class *class;
  unsigned int instance_count;    // how many drivers have been instantiated?
  struct list_head instance_list; // pointer to first instance
};

// allocate and initialize global data (class level)
static struct esl_oled_driver driver_data = {
    .first_devno = 0,
    .instance_count = 0,
    .instance_list = LIST_HEAD_INIT(driver_data.instance_list),
};

// find instance from inode using minor number and linked list
static struct esl_oled_instance *inode_to_instance(struct inode *i)
{
  struct esl_oled_instance *inst_iter;
  unsigned int minor = iminor(i);

  // start with fist element in linked list (stored at class),
  // and iterate through its elements
  list_for_each_entry(inst_iter, &driver_data.instance_list, inst_list)
  {
    // found matching minor number?
    if (MINOR(inst_iter->devno) == minor)
    {
      // return instance pointer of corresponding instance
      return inst_iter;
    }
  }

  // not found
  return NULL;
}

// return instance struct based on file
static struct esl_oled_instance *file_to_instance(struct file *f)
{
  return inode_to_instance(f->f_path.dentry->d_inode);
}

/* Character device File Ops */
static ssize_t esl_oled_write(struct file *f,
                              const char __user *buf, size_t len,
                              loff_t *offset)
{
  ssize_t written;

  return written;
}

// definition of file operations
struct file_operations esl_oled_fops = {
    .write = esl_oled_write,
};

// define the class, creates /sys/class/audio
static struct class esl_oled_class = {
    .name = "oled",
    .owner = THIS_MODULE,
};

/* OLED interrupt handler */
static irqreturn_t esl_oled_irq_handler(int irq, void *dev_id)
{
  struct esl_oled_instance *inst = dev_id;

  return IRQ_HANDLED;
}

/* Button interrupt handler */
static irqreturn_t esl_button_irq_handler(int irq, void *dev_id)
{
  struct esl_oled_instance *inst = dev_id;

  printk(KERN_INFO "Button interrupt");

  return IRQ_HANDLED;
}

// enable interrupts for buttons
static int enable_button_interrupts(struct esl_oled_instance *inst)
{
  int err;
  int i;
  struct gpio_chip* chip;
  struct platform_device* pdev;
  struct device* dev;
  struct gpio_desc* gdesc;
  struct resource *res;

  // find GPIO description by GPIO number passed
  gdesc = gpio_to_desc(BTN_GPIO_ADDR);
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

  printk(KERN_INFO "pdev name: %s\n", pdev->name);
  printk(KERN_INFO "pdev id: %d\n", pdev->id);
  printk(KERN_INFO "pdev num resources: %d\n", pdev->num_resources);

  // Acquire the correct hardware interrupt number
  res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
  if (!res) {
    printk(KERN_ERR "Failed to get IRQ resource\n");
    return -ENODEV;
  }
  button_irqnum = res->start;

  // printk(KERN_INFO "IRQ number: %d\n", irqnum);
  // printk(KERN_INFO "IRQ flags: %lu\n", res->flags);
  // printk(KERN_INFO "IRQ start: %lu\n", res->start);
  // printk(KERN_INFO "IRQ end: %lu\n", res->end);
  // printk(KERN_INFO "IRQ name: %s\n", res->name);
  // printk(KERN_INFO "Resource size: %lu\n", resource_size(res));
  // printk(KERN_INFO "Memory region start address: %pa\n", &res->start);

  // Request an operating system interrupt
  err = devm_request_irq(&pdev->dev, res->start,
                         esl_button_irq_handler,
                         IRQF_TRIGGER_HIGH,
                         "zedbtn", inst);
  if (err < 0)
  {
    return err;
  }

  // get platform resource
  res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (!res) {
    printk(KERN_ERR "Failed to get IRQ resource\n");
    return -ENODEV;
  }

  // Retrieve memory region and map to button_regs
  button_regs = devm_ioremap(dev, res->start, resource_size(res));
  if (IS_ERR(button_regs))
  {
    return PTR_ERR(button_regs);
  }
  

  // // Enable interrupts in the AXI GPIO controller
  // enable_interrupts();
  // Set tri buffer to 1
  iowrite32(0x01, button_regs + GPIO_TRI);
  printk(KERN_INFO "GPIO_TRI: %x\n", ioread32(button_regs + GPIO_TRI));

  // enable global interrupt in the GIER register, set bit 31
  iowrite32(0x80000000, button_regs + GIER_REG);
  printk(KERN_INFO "GIER: %x\n", ioread32(button_regs + GIER_REG));

  // 1. enable all IER bits
  iowrite32(0x3, button_regs + IER_REG);
  printk(KERN_INFO "IER: %x\n", ioread32(button_regs + IER_REG));


  // initialize GPIOs
  // initialize_gpio();
  for (i = 0; i < BUTTON_GPIO_COUNT; ++i) {
    gpio_direction_input(BTN_GPIO_ADDR + i);
  }

  return 0;
}

static int esl_oled_probe(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = NULL;
  int err;
  struct resource *res;
  struct device *dev;

  printk(KERN_INFO "start probe");

  dev_t devno = MKDEV(driver_data.first_devno, driver_data.instance_count);

  // allocate instance
  inst = devm_kzalloc(&pdev->dev, sizeof(struct esl_oled_instance),
                      GFP_KERNEL);

  if (!inst)
  {
    // ran out of memory
    return -ENOMEM;
  }

  // set platform driver data
  platform_set_drvdata(pdev, inst);

  // get registers
  res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (IS_ERR(res))
  {
    return PTR_ERR(res);
  }

  inst->regs = devm_ioremap_resource(&pdev->dev, res);
  if (IS_ERR(inst->regs))
  {
    return PTR_ERR(inst->regs);
  }

  // get fifo depth
  err = of_property_read_u32(pdev->dev.of_node, "fifo-size",
                             &inst->fifo_size);
  if (err)
  {
    printk(KERN_ERR "%s: failed to retrieve OLED fifo size\n",
           DRIVER_NAME);
    return err;
  }

  // get interrupt
  res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
  if (IS_ERR(res))
  {
    return PTR_ERR(res);
  }

  err = devm_request_irq(&pdev->dev, res->start,
                         esl_oled_irq_handler,
                         IRQF_TRIGGER_HIGH,
                         "zedoled", inst);
  if (err < 0)
  {
    return err;
  }

  // save irq number
  inst->irqnum = res->start;

  // create character device

  // get device number
  inst->devno = MKDEV(MAJOR(driver_data.first_devno),
                      driver_data.instance_count);

  printk(KERN_INFO "Device probed with devno #%u", inst->devno);
  printk(KERN_INFO "Device probed with inst count #%u", driver_data.instance_count);

  cdev_init(&inst->chr_dev, &esl_oled_fops);
  inst->chr_dev.owner = THIS_MODULE;
  err = cdev_add(&inst->chr_dev, inst->devno, 1);
  if (err)
  {
    printk(KERN_ERR "Failed to add cdev\n");
    return err;
  }

  // Device creation for sysfs
  dev = device_create(driver_data.class, &pdev->dev, inst->devno, NULL, "zedoled%d", MINOR(inst->devno));
  if (IS_ERR(dev))
  {
    cdev_del(&inst->chr_dev);
    return PTR_ERR(dev);
  }

  printk(KERN_INFO "after device_create");

  // enable button interrupts
  err = enable_button_interrupts(inst);
  if (err)
  {
    return err;
  }

  // increment instance count
  driver_data.instance_count++;

  // put into list
  INIT_LIST_HEAD(&inst->inst_list);
  list_add(&inst->inst_list, &driver_data.instance_list);

  return 0;
}

static int esl_oled_remove(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = platform_get_drvdata(pdev);

  // cleanup and remove
  // cdev_del(&inst->chr_dev);
  device_destroy(&esl_oled_class, inst->devno);

  // remove from list
  list_del(&inst->inst_list);

  return 0;
}

// platform driver definition
static struct platform_driver esl_oled_driver = {
    .probe = esl_oled_probe,
    .remove = esl_oled_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(esl_oled_of_ids),  
    },
};

static int esl_oled_init(void)
{
  int err;

  // alocate character device region
  err = alloc_chrdev_region(&driver_data.first_devno, 0, 1, "zedoled");
  if (err < 0)
  {
    return err;
  }

  // although not using sysfs, still necessary in order to automatically
  // get device node in /dev
  driver_data.class = class_create(THIS_MODULE, "zedoled");
  if (IS_ERR(driver_data.class))
  {
    return -ENOENT;
  }

  platform_driver_register(&esl_oled_driver);

  printk(KERN_INFO "Sucessfully initialized OLED module");

  return 0;
}

static void esl_oled_exit(void)
{

  // free character device region
  unregister_chrdev_region(driver_data.first_devno, 1);

  // remove class
  class_destroy(driver_data.class);

  // plat driver unregister
  platform_driver_unregister(&esl_oled_driver);

  printk(KERN_INFO "Sucessfully exited OLED module");
}

module_init(esl_oled_init);
module_exit(esl_oled_exit);

MODULE_DESCRIPTION("ZedBoard Oled driver");
MODULE_LICENSE("GPL");
