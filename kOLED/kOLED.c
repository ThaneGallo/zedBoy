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

// docs.xilinx.com/r/en-US/pg153-axi-quad-spi/Port-Descriptions
// spi register space
static int SRR = 0x40;        // software reg reset
static int SPICR = 0x60;      // spi cont reg
static int SPISR = 0x64;      // spi stat reg
static int SPIDTR = 0x68;     // spi data trans reg
static int SPIDRR = 0x6C;     // spi data rec reg
static int SPISSR = 0x70;     // slave select
static int SPI_TXFIFO = 0x78; // occupancy tx
static int SPI_RXFIFO = 0x74; // occupancy rx

// interrupts
static int DGIER = 0x1C; //  dev global ier
static int IPISR = 0x20; // ip isr
static int IPIER = 0x28; // ip ier

#define DISPLAY_BUF_SZ 512 /* 32 x 128 bit monochrome  == 512 bytes        */
#define MAX_LINE_LEN 16    /* 128 bits wide and current char width is 8 bit */
#define MAX_ROW 4
#define OLED_MAX_PG_CNT 4 /* number of display pages in OLED controller */
#define OLED_CONTROLLER_PG_SZ 128
#define OLED_CONTROLLER_CMD 0
#define OLED_CONTROLLER_DATA 1

/* commands for the OLED display controller	*/
#define OLED_SET_PG_ADDR 0x22
#define OLED_DISPLAY_OFF 0xAE
#define OLED_DISPLAY_ON 0xAF
#define OLED_CONTRAST_CTRL 0x81
#define OLED_SET_PRECHARGE_PERIOD 0xD9
#define OLED_SET_SEGMENT_REMAP 0xA1
#define OLED_SET_COM_DIR 0xC8
#define OLED_SET_COM_PINS 0xDA

#define OLED_CTRL_REG 0x41200000

static void __iomem *ctrl_reg_global;

struct esl_oled_instance
{
  void *__iomem spi_regs;  // oled registers
  void *__iomem ctrl_regs; // oled registers

  struct oled_ctrl;

  struct mutex mutex; // lock/unlock

  /* Display Buffers */
  uint8_t disp_on;
  uint8_t *disp_buf;

  struct spi_device *spi;
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

  struct esl_oled_instance *inst = file_to_instance(f);
  ssize_t written = 0;
  unsigned int space;
  unsigned int to_write;
  int err, i;

  if (!inst)
  {
    // instance not found
    return -ENOENT;
  }

  to_write = min((size_t)space, len);

  // Copy from user space to kernel buffer
  if (copy_from_user(inst->fifo_buf, buf + written, to_write))
  {
    return -EFAULT;
  }

  // Write to AXI FIFO
  for (i = 0; i < to_write; i += 4)
  {
    // iowrite32(*(u32 *)(inst->fifo_buf + i), inst->regs + TDFD);
  }

  // iowrite32(to_write, inst->regs + TLR);

  written += to_write;
  len -= to_write;

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

  // interrupt errors
  unsigned long TX_HALF_EMPTY = 0x00000040;
  unsigned long DTR_UNDERRUN = 0x00000004;
  unsigned long DTR_EMPTY = 0x00000003;
  unsigned long SLAVE_MODF = 0x00000002;
  unsigned long MODF = 0x00000001;

  // Reads the interrupt status
  ISR_reg = ioread32(inst->spi_regs + IPISR);

  if (ISR_reg & (1 << MODF))
  {
    printk(KERN_WARNING "mode error\n");
    return -EINVAL;
  }

  else if (ISR_reg & (1 << SLAVE_MODF))
  {
    printk(KERN_WARNING "slave mode error\n");
    return -EINVAL;
  }

  else if (ISR_reg & (1 << DTR_UNDERRUN))
  {
    printk(KERN_WARNING "DTR_underrun!\n");
    return -EINVAL;
  }

  else if (ISR_reg & (1 << DTR_EMPTY))
  {
    printk(KERN_WARNING "DTR empty!\n");
    return -EINVAL;
  }

  else if (ISR_reg & (1 << TX_HALF_EMPTY))
  {
    // if half empty wait a little to fill back up
    printk(KERN_WARNING "tx fifo half empty!\n");
    usleep_range(1000, 2000);
    continue;
  }

  return IRQ_HANDLED;
}

static int oled_on(struct platform_device *pdev)
{

  // folows setup instructions as indicated in datasheet

  struct esl_oled_instance *inst = platform_get_drvdata(pdev);

  uint32_t gpio;

  gpio = ioread32(inst->ctrl_regs);

  // sets vdd to 1
  gpio = gpio | (1 << 3);
  iowrite32(gpio, inst->ctrl_regs);

  // sets res# to low
  gpio = gpio & ~(1 << 1);
  iowrite32(gpio, inst->ctrl_regs);

  // res switch delay
  usleep(5);

  // sets res# to high
  gpio = gpio | (1 << 1);
  iowrite32(gpio, inst->ctrl_regs);

  // power on VCC
  gpio = gpio | (1 << 2);
  iowrite32(gpio, inst->ctrl_regs);

  iowrite32(OLED_DISPLAY_ON ,inst->spi_regs + DTR);

  // turn on time
  usleep(100000);

  return 0;
}

static int oled_off(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = platform_get_drvdata(pdev);

  uint32_t gpio;

  iowrite32(OLED_DISPLAY_OFF, inst->spi_regs + DTR)

  gpio = ioread32(inst->ctrl_regs);

  // power off VCC
  gpio = gpio & ~(1 << 2);
  iowrite32(gpio, inst->ctrl_regs);

  // t_off time
  usleep(100000);

  // power off VDD
  gpio = gpio & ~(1 << 3);
  iowrite32(gpio, inst->ctrl_regs);

  return 0;
}

static int oled_spi_setup(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = platform_get_drvdata(pdev);

  unsigned long tx_rst, master_mode, CPOL, CPHA, SCK_SETUP;
  unsigned long TX_FIFO_ALL, DRR_NOT_EMPTY, ITR_EN;

  tx_rst = 0x00000010;
  master_mode = 0x00000002;

  // clocking selection
  CPOL = 0x00000000;
  CPHA = 0x00000000;

  SCK_SETUP = CPOL | CPHA;

  // itr standard
  DRR_NOT_EMPTY = 0x00000080; //?
  TX_FIFO_ALL = 0x000000AF;
  ITR_EN = DRR_NOT_EMPTY | TX_FIFO_ALL;

  SPICR_SETUP = SCK_SETUP | tx_rst | master_mode;

  // software reset
  iowrite32(0x0000000a, inst->spi_regs + SRR);

  // resets tx register
  iowrite32(SPICR_SETUP, inst->spi_regs + SPICR);

  // interrupt enable
  iowrite32(ITR_EN, inst->spi_regs + DGIER);

  return 0;
}

static int esl_oled_probe(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = NULL;
  int err;
  struct resource *res;
  struct device *dev;

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

  inst->spi_regs = devm_ioremap_resource(&pdev->dev, res);
  if (IS_ERR(inst->spi_regs))
  {
    return PTR_ERR(inst->spi_regs);
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

  // printk(KERN_INFO "Device probed with devno #%u", inst->devno);
  // printk(KERN_INFO "Device probed with inst count #%u", driver_data.instance_count);
  // printk(KERN_INFO "Device probed with address #%p", inst->regs);

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

  // increment instance count
  driver_data.instance_count++;

  // put into list
  INIT_LIST_HEAD(&inst->inst_list);
  list_add(&inst->inst_list, &driver_data.instance_list);

  inst->ctrl_regs = ctrl_reg_global;

  oled_on();
  oled_spi_setup();

  return 0;
}

static int esl_oled_remove(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = platform_get_drvdata(pdev);

  oled_off();

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
        .of_match_table = of_match_ptr(esl_oled_of_ids),
    },
};

static int oled_chip_setup(unsigned long addr)
{

  int err;
  struct gpio_chip *chip;
  struct platform_device *pdev;
  struct device *dev;
  struct gpio_desc *gdesc;
  struct resource *res;

  // find GPIO description by GPIO number passed
  gdesc = gpio_to_desc(addr);
  if (!gdesc)
  {
    printk("error getting GPIO description for oled!\n");
    return -ENODEV;
  }

  // get chip of switches gpio
  chip = gpiod_to_chip(gdesc);
  if (!chip)
  {
    printk("error getting oled GPIO chip\n");
    return -ENODEV;
  }

  // retrieve original platform device from gpio chip,
  // now we can extract information from the device tree node
  dev = chip->parent;
  pdev = to_platform_device(dev);

  res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (res == NULL)
  {
    return -ENOMEM;
  }

  ctrl_reg_global = devm_ioremap(dev, res->start, resource_size(res));

  if (IS_ERR(ctrl_reg_global))
  {
    return -PTR_ERR(ctrl_reg_global);
  }

  return 0;
}

static int esl_oled_init(void)
{
  int err;
  unsigned long addr = 0x41200000;

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

  oled_chip_setup(addr);

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
