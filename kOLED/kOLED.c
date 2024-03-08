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


#define DRIVER_NAME "esl-oled"


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

  // fifo depth
  unsigned int tx_fifo_depth;

  // fifo buffer
  unsigned char fifo_buf[31];

  // wait queue
  wait_queue_head_t waitq;

};

// matching table
static struct of_device_id esl_audio_of_ids[] = {
    {.compatible = "xlnx,xps-spi-2.00.a"},
    {}};

// structure for class of all audio drivers
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

/* interrupt handler */
static irqreturn_t esl_oled_irq_handler(int irq, void *dev_id)
{
  struct esl_oled_instance *inst = dev_id;
 
  return IRQ_HANDLED;
}

static int esl_oled_probe(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = NULL;
  int err;
  struct resource *res;
  struct device *dev;
  const void *prop;
  struct device_node *i2sctl_node;

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

  // get registers (AXI FIFO)
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

  // get TX fifo depth
  err = of_property_read_u32(pdev->dev.of_node, "xlnx,tx-fifo-depth",
                             &inst->tx_fifo_depth);
  if (err)
  {
    printk(KERN_ERR "%s: failed to retrieve TX fifo depth\n",
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

  printk(KERN_INFO "inst->devno minor %u", MINOR(inst->devno));

  // Device creation for sysfs
  dev = device_create(driver_data.class, &pdev->dev, inst->devno, NULL, "zedoled%d", MINOR(inst->devno));
  if (IS_ERR(dev))
  {
    cdev_del(&inst->chr_dev);
    return PTR_ERR(dev);
  }

  printk(KERN_INFO "after device_create");

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
        .of_match_table = of_match_ptr(esl_audio_of_ids),// not sure what this line is for
    },
};

static int esl_oled_init(void)
{
  int err;

  // alocate character device region
  err = alloc_chrdev_region(&driver_data.first_devno, 0, 16, "zedaudio");
  if (err < 0)
  {
    return err;
  }



  // although not using sysfs, still necessary in order to automatically
  // get device node in /dev
  driver_data.class = class_create(THIS_MODULE, "zedaudio");
  if (IS_ERR(driver_data.class))
  {
    return -ENOENT;
  }

  platform_driver_register(&esl_oled_driver);

  return 0;
}

static void esl_oled_exit(void)
{

  // free character device region
  unregister_chrdev_region(driver_data.first_devno, 16);

  // remove class
  class_destroy(driver_data.class);

  // plat driver unregister
  platform_driver_unregister(&esl_oled_driver);

}

module_init(esl_oled_init);
module_exit(esl_oled_exit);

MODULE_DESCRIPTION("ZedBoard Oled driver");
MODULE_LICENSE("GPL");
