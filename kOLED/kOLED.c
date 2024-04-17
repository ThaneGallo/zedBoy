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

// docs.xilinx.com/r/en-US/pg153-axi-quad-spi/Port-Descriptions
// spi register space
#define SRR 0x40        // software reg reset
#define SPICR = 0x60;      // spi cont reg
#define SPISR = 0x64;      // spi stat reg
#define DTR = 0x68;        // spi data trans reg
#define DRR = 0x6C;     // spi data rec reg
#define SPI_TXFIFO = 0x78; // occupancy tx

// interrupts
static int DGIER = 0x1C; //  dev global ier
static int IPISR = 0x20; // ip isr
static int IPIER = 0x28; // ip ier

static void __iomem *gpio_regs_global;

/* commands for the OLED display controller	*/
#define OLED_SET_PG_ADDR 0x22
#define OLED_DISPLAY_OFF 0xAE
#define OLED_DISPLAY_ON 0xAF
#define OLED_CONTRAST_CTRL 0x81
#define OLED_SET_PRECHARGE_PERIOD 0xD9
#define OLED_SET_SEGMENT_REMAP 0xA1
#define OLED_SET_COM_DIR 0xC8
#define OLED_SET_COM_PINS 0xDA

#define DISPLAY_BUFFER 0xA4

#define OLED_CTRL_REG 0x41200000
#define GPIO_DATA 0x0008
#define GPIO_TRI 0x000C

struct esl_oled_instance
{
  void *__iomem spi_regs;  // oled registers
  void *__iomem gpio_regs; // oled registers

  struct mutex mutex; // lock/unlock

  /* Display Buffers */
  uint8_t disp_on;
  uint8_t *disp_buf;

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
  unsigned long gpio;
  int space;
  ssize_t written;
  
  gpio = ioread32(inst->gpio_regs + GPIO_DATA);

  //sets to recieve data
  gpio = gpio | (1);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);

  if(!inst){
    return -ENOENT;
  }

   while (len > 0)
  {
    space = ioread32(inst->spi_regs + SPI_TXFIFO);

    if(space == 0){
      
    }

  }


  //sets to recieve command
  gpio = gpio & ~(1);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);


  //writes full buffer to OLED
  iowrite32(OLED_DISPLAY_ON, inst->spi_regs + DTR);


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
  unsigned long TX_EMPTY = 0x00000040;
  unsigned long DTR_UNDERRUN = 0x00000004;
  unsigned long DTR_EMPTY = 0x00000003;
  unsigned long SLAVE_MODF = 0x00000002;
  unsigned long MODF = 0x00000001;

  unsigned long ISR_reg;

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
  }

  return IRQ_HANDLED;
}

static int oled_on(struct esl_oled_instance *inst)
{
  // folows setup instructions as indicated in datasheet
  uint32_t gpio;

  gpio = ioread32(inst->gpio_regs + GPIO_DATA);

  printk(KERN_INFO "%x\n", gpio);

  // sets vdd to 1
  gpio = gpio & ~(1 << 3);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);

  printk(KERN_INFO "%x\n", gpio);

  // sets res# to low
  gpio = gpio & ~(1 << 1);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);
  printk(KERN_INFO "%x\n", gpio);

  // res switch delay
  usleep_range(1000,1000);

  // sets res# to high
  gpio = gpio | (1 << 1);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);
  printk(KERN_INFO "%x\n", gpio);

  // res switch delay
  usleep_range(10000,10000);

  // sets res# to low
  gpio = gpio & ~(1 << 1);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);
  printk(KERN_INFO "%x\n", gpio);

  // power on VCC
  gpio = gpio & ~(1 << 2);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);
  printk(KERN_INFO "%x\n", gpio);

  //data = 1
  //command = 0

  // set input to command
  gpio = gpio & ~(1);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);
  printk(KERN_INFO "%x\n", gpio);

  iowrite32(0xAE, inst->spi_regs + DTR);

  // turn on time
  usleep_range(100000, 100000);

  return 0;
}

static int oled_setup(struct esl_oled_instance *inst){
  uint32_t gpio;

  gpio = ioread32(inst->gpio_regs + GPIO_DATA);

  // set input to command
  gpio = gpio & ~(1);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);

  //set display off
  iowrite32(0xAE, inst->spi_regs + DTR);

  // set display clock divide ratio
  iowrite32(0xD5, inst->spi_regs + DTR);
  iowrite32(0x80, inst->spi_regs + DTR);

  // set multiplex ratio
  iowrite32(0xA8, inst->spi_regs + DTR);
  iowrite32(0x1F, inst->spi_regs + DTR);

  // set display offset 
  iowrite32(0xD3, inst->spi_regs + DTR);
  iowrite32(0x00, inst->spi_regs + DTR);

  // set display start line 
  iowrite32(0x40, inst->spi_regs + DTR);
  
  // set charge pump
  iowrite32(0x8D, inst->spi_regs + DTR);
  iowrite32(0x10, inst->spi_regs + DTR);
  
  // set segment remap
  iowrite32(0xA1, inst->spi_regs + DTR);
  
  // set COM output scan direction
  iowrite32(0xC8, inst->spi_regs + DTR);
  
  // set com pins / hardware config
  iowrite32(0xDA, inst->spi_regs + DTR);
  iowrite32(0x02, inst->spi_regs + DTR);

  // set contrast control
  iowrite32(0x81, inst->spi_regs + DTR);
  iowrite32(0x8F, inst->spi_regs + DTR);
  
  // set pre-charge period
  iowrite32(0xD9, inst->spi_regs + DTR);
  iowrite32(0x22, inst->spi_regs + DTR);

  //set VCOMH delselect level
  iowrite32(0xDB, inst->spi_regs + DTR);
  iowrite32(0x40, inst->spi_regs + DTR);

  //entire display on
  iowrite32(0xA4, inst->spi_regs + DTR);

  //normal display / inverse
  iowrite32(0xA6, inst->spi_regs + DTR);

  return 0;

}

static int oled_off(struct esl_oled_instance *inst)
{

  uint32_t gpio;

  gpio = ioread32(inst->gpio_regs);


   // set input to command
  gpio = gpio & ~(1);
  iowrite32(gpio, inst->gpio_regs);
  iowrite32(OLED_DISPLAY_OFF, inst->spi_regs + DTR);



  // power off VCC
  gpio = gpio & ~(1 << 2);
  iowrite32(gpio, inst->gpio_regs);

  // t_off time
  usleep_range(100000, 100001);

  // power off VDD
  gpio = gpio & ~(1 << 3);
  iowrite32(gpio, inst->gpio_regs);

  return 0;
}

static int oled_spi_setup(struct esl_oled_instance *inst)
{

  unsigned long tx_rst, master_mode, CPOL, SPE, CPHA, SCK_SETUP, SPICR_SETUP;
  unsigned long TX_FIFO_ALL, DRR_NOT_EMPTY, ITR_EN;

  tx_rst = 1 << 5;
  master_mode = 1 << 2;
  SPE = 1 << 1;

  // clocking selection
  CPOL = 0x00000000;
  CPHA = 0x00000000;

  SCK_SETUP = CPOL | CPHA;

  // itr standard
  DRR_NOT_EMPTY = 0x00000080; //?
  TX_FIFO_ALL = 0x000000AF;
  ITR_EN = DRR_NOT_EMPTY | TX_FIFO_ALL;

  SPICR_SETUP = SCK_SETUP | tx_rst | master_mode | SPE;

  // software reset
  iowrite32(0x0000000a, inst->spi_regs + SRR);

  // resets tx register
  iowrite32(SPICR_SETUP, inst->spi_regs + SPICR);

  // interrupt enable
  iowrite32(ITR_EN, inst->spi_regs + DGIER);

  return 0;
}

static int oled_setup_full(struct esl_oled_instance *inst){

  uint32_t gpio;

  gpio = ioread32(inst->gpio_regs + GPIO_DATA);

  // sets vdd to 1
  gpio = gpio & ~(1 << 3);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);

  //set display off
  iowrite32(0xAE, inst->spi_regs + DTR);

  // set display clock divide ratio
  iowrite32(0xD5, inst->spi_regs + DTR);
  iowrite32(0x80, inst->spi_regs + DTR);

  // set multiplex ratio
  iowrite32(0xA8, inst->spi_regs + DTR);
  iowrite32(0x1F, inst->spi_regs + DTR);

  // set display offset 
  iowrite32(0xD3, inst->spi_regs + DTR);
  iowrite32(0x00, inst->spi_regs + DTR);

  // set display start line 
  iowrite32(0x40, inst->spi_regs + DTR);
  
  // set charge pump
  iowrite32(0x8D, inst->spi_regs + DTR);
  iowrite32(0x10, inst->spi_regs + DTR);
  
  // set segment remap
  iowrite32(0xA1, inst->spi_regs + DTR);
  
  // set COM output scan direction
  iowrite32(0xC8, inst->spi_regs + DTR);
  
  // set com pins / hardware config
  iowrite32(0xDA, inst->spi_regs + DTR);
  iowrite32(0x02, inst->spi_regs + DTR);

  // set contrast control
  iowrite32(0x81, inst->spi_regs + DTR);
  iowrite32(0x8F, inst->spi_regs + DTR);
  
  // set pre-charge period
  iowrite32(0xD9, inst->spi_regs + DTR);
  iowrite32(0x22, inst->spi_regs + DTR);

  //set VCOMH delselect level
  iowrite32(0xDB, inst->spi_regs + DTR);
  iowrite32(0x40, inst->spi_regs + DTR);

  //normal display / inverse
  iowrite32(0xA6, inst->spi_regs + DTR);

  // power on VCC
  gpio = gpio & ~(1 << 2);
  iowrite32(gpio, inst->gpio_regs + GPIO_DATA);

  usleep_range(100000, 100000);

    //entire display on
  iowrite32(0xA4, inst->spi_regs + DTR);


  return 0;
}

static int esl_oled_probe(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = NULL;
  int err;
  struct resource *res;
  struct device *dev;

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
  // err = of_property_read_u32(pdev->dev.of_node, "fifo-size",
  //                            &inst->fifo_size);
  // if (err)
  // {
  //   printk(KERN_ERR "%s: failed to retrieve OLED fifo size\n",
  //          DRIVER_NAME);
  //   return err;
  // }

  // // get interrupt
  // res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
  // if (IS_ERR(res))
  // {
  //   return PTR_ERR(res);
  // }

  // err = devm_request_irq(&pdev->dev, res->start,
  //                        esl_oled_irq_handler,
  //                        IRQF_TRIGGER_HIGH,
  //                        "zedoled", inst);
  // if (err < 0)
  // {
  //   return err;
  // }

  // save irq number
  // inst->irqnum = res->start;

  // create character device
  // increment instance count
  driver_data.instance_count++;

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
  dev = device_create(driver_data.class, NULL, inst->devno, NULL, "zedoled%d", MINOR(inst->devno));
  if (IS_ERR(dev))
  {
    cdev_del(&inst->chr_dev);
    return PTR_ERR(dev);
  }

  // put into list
  INIT_LIST_HEAD(&inst->inst_list);
  list_add(&inst->inst_list, &driver_data.instance_list);


//sets tri state control regs to ctrl reg global
  inst->gpio_regs = ioremap(OLED_CTRL_REG, 0x128);

  if (!inst->gpio_regs)
  {
    printk(KERN_ERR "Failed to map physical address\n");
    return -ENOMEM;
  }


  // oled_spi_setup(inst);

  // // makes all gpio pins into outputs
  // iowrite32(0 ,inst->gpio_regs + GPIO_TRI);

  // oled_setup_full(inst);

  // // oled_on(inst);

  // // oled_setup(inst);

  // iowrite32(0xA5, inst->spi_regs + DTR);

  
  return 0;
}

static int esl_oled_remove(struct platform_device *pdev)
{
  struct esl_oled_instance *inst = platform_get_drvdata(pdev);

  // oled_off(inst);

  cdev_del(&inst->chr_dev);
 
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


  printk(KERN_INFO "Sucessfully initialized OLED module\n\n");

  return 0;
}

static void esl_oled_exit(void)
{

  //unmaps control registers
  iounmap(gpio_regs_global);

  // free character device region
  unregister_chrdev_region(driver_data.first_devno, 1);

  // remove class
  class_destroy(driver_data.class);

  // plat driver unregister
  platform_driver_unregister(&esl_oled_driver);

  printk(KERN_INFO "Sucessfully exited OLED module\n");
}

module_init(esl_oled_init);
module_exit(esl_oled_exit);

MODULE_DESCRIPTION("ZedBoard OLED driver");
MODULE_LICENSE("GPL");
