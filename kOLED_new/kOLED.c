
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
#define SRR 0x40    // software reg reset
#define SPICR 0x60  // spi cont reg
#define SPISR 0x64  // spi stat reg
#define DTR 0x68    // spi data trans reg
#define DRR 0x6C    // spi data rec reg
#define SPISSR 0x70 // spi slave select
#define SPITXOCC 0x74
#define SPIRXOCC 0x78
#define SPI_TXFIFO 0x78 // occupancy tx

// gpio macros
#define OLED_DC (1 << 0)   // data / command
#define OLED_RES (1 << 1)  // reset pin
#define OLED_VBAT (1 << 2) // vcc
#define OLED_VDD (1 << 3)  // vdd

// interrupts
#define DGIER 0x1C //  dev global ier
#define IPISR 0x20 // ip isr
#define IPIER 0x28 // ip ier



// IPISR interrput bits
#define IP_MODF (1 << 0)
#define IP_SLAVE_MODF (1 << 1)
#define IP_TX_EMPTY (1 << 2)
#define TX_UNDERRUN (1 << 3)
#define IP_IP_RX_FULL (1 << 4)
#define IP_RX_OVERRUN (1 << 5)
#define IP_TX_HALF_EMPTY (1 << 6)
#define IP_SLAVE_SELECT (1 << 7)
#define IP_RX_NOT_EMPTY (1 << 8)
#define IP_CPOL_CPHA (1 << 9)

//SPISR
#define RX_EMPTY (1 << 0)
#define RX_FULL (1 << 1)
#define TX_EMPTY (1 << 2)
#define TX_FULL (1 << 3)
#define MODF (1 << 4)
#define SLAVE_MODE_SELECT (1 << 5)
#define CPOL_CPHA (1 << 6)
#define SLAVE_ERROR (1 << 7)


// SPICR thingies
#define MASTER_INHIBIT (1 << 8)
#define SLAVE_ASSERT (1 << 7)
#define RX_RESET (1 << 6)
#define TX_RESET (1 << 5)
#define MASTER_MODE (1 << 2)
#define SPE (1 << 1)

/* commands for the OLED display controller	*/
#define OLED_ADDR_MODE 0x20
#define OLED_SET_COL 0x21
#define OLED_SET_PG 0x22
#define OLED_CONTRAST 0x81
#define OLED_SCREEN_ON 0xA5
#define OLED_DISABLE 0xAE
#define OLED_ENABLE 0xAF

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
    void *__iomem spi_regs;  // spi registers
    void *__iomem gpio_regs; // oled registers

    struct cdev chr_dev; // character device

    unsigned char fifo_buf[15];

    unsigned int irqnum;

    dev_t devno;

    // list head
    struct list_head inst_list;
};

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

static inline void reg_write(void *__iomem base, u32 reg, u32 value)
{
    iowrite32(value, base + reg);
}

static inline u32 reg_read(void *__iomem base, u32 reg)
{
    return ioread32(base + reg);
}

static void print_debug(void *__iomem base)
{
    printk(KERN_INFO "SPICR | 0x%08X\n", reg_read(base, SPICR));
    printk(KERN_INFO "SPISR | 0x%08X\n", reg_read(base, SPISR));
    printk(KERN_INFO "SPISSR | 0x%08X\n", reg_read(base, SPISSR));

    if (reg_read(base, SPISR) & TX_EMPTY)
    {
        printk(KERN_DEBUG "SPITXOCC | 0x%0X\n", reg_read(base, SPITXOCC));
    }
    else
    {
        printk(KERN_DEBUG "SPITXOCC | 0x%0X\n", reg_read(base, SPITXOCC) + 1);
    }

    printk(KERN_DEBUG "IPISR | 0x%0X\n", reg_read(base, IPISR));
}

static int spi_transmit(void *__iomem base, unsigned char *buf, unsigned int size, unsigned char *recBuf)
{
    unsigned int spisr, recieved = 0;

    printk(KERN_INFO "SPISR beginning of spi| 0x%08X\n", reg_read(base, IPISR));

    // setups master mode and inhibit
    reg_write(base, SPICR, reg_read(base, SPICR) | MASTER_MODE | MASTER_INHIBIT);

    // clear both fifos
    reg_write(base, SPICR, reg_read(base, SPICR) | TX_RESET | RX_RESET);

    printk(KERN_INFO "post reset | 0x%08X\n", reg_read(base, IPISR));


    // asserts slave
    reg_write(base, SPISSR, 0x00);

    // print_debug(base);

    // printk(KERN_INFO "size before loop %i", size);

    // writes buffer to tx fifo
    while (size--)
    {
        printk(KERN_INFO "send 0x%x\n", *buf);
        reg_write(base, DTR, *(buf++));
        printk(KERN_INFO " in loop | 0x%08X\n", reg_read(base, IPISR));
        
    }

    // disable master mode and enable SPE
    reg_write(base, SPICR, (reg_read(base, SPICR) & ~(MASTER_INHIBIT)) | SPE); 


    // 10us
    usleep_range(10000, 10000);

    while (!(reg_read(base, SPISR) & TX_EMPTY))
    {

        if (recBuf && !(reg_read(base, SPISR) & RX_EMPTY))
        {
             reg_write(base, SPICR, reg_read(base, SPICR) | RX_RESET);
        }
    }

  

    // in case anything is left in rx fifo
    while (recBuf && !(reg_read(base, SPISR) & RX_EMPTY))
    {
        reg_write(base, SPICR, reg_read(base, SPICR) | RX_RESET);
    }

    // deselect slave
    reg_write(base, SPISSR, 0xFF);

    reg_write(base, SPICR, (reg_read(base, SPICR) | (MASTER_INHIBIT)) & ~(SPE));

    // if (reg_read(base, SPISR) & RX_EMPTY)
    // {
    //     printk(KERN_DEBUG "EMPTY recieve | 0x%0X\n", reg_read(base, SPITXOCC));
    // }

    // if (reg_read(base, SPISR) & TX_EMPTY)
    // {
    //     printk(KERN_DEBUG "EMPTY transmit | 0x%0X\n", reg_read(base, SPIRXOCC));
    // }

    // print_debug(base);

    return recieved;
}

/* Character device File Ops */
static ssize_t esl_oled_write(struct file *f,
                              const char __user *buf, size_t len,
                              loff_t *offset)
{
    struct esl_oled_instance *inst = file_to_instance(f);
    unsigned int sent, to_send, recieved;
    char recBuf[10];

    if (!inst)
    {
        return -ENOMEM;
    }

    //  // Copy from user space to kernel buffer
    // if (copy_from_user(inst->fifo_buf, buf + written, to_write))
    // {
    //   return -EFAULT;
    // }

    // transmits buffer in chunks (fifo size)
    if (len > 16)
    {
        sent = 0;

        while (sent < len)
        {
            to_send = len - sent;
            if (to_send > 16)
            {
                to_send = 16;
            }
            // recieved += spi_transmit(inst->spi_regs, buf + sent, to_send, revBuf);
            sent += to_send;
        }
    }
    // transmits whole buffer
    else
    {
        recieved += spi_transmit(inst->spi_regs, buf, len, NULL);
    }

    return 0;
}

// definition of file operations
struct file_operations esl_oled_fops = {
    .write = esl_oled_write,
};

void spi_send_buffer(struct esl_oled_instance *inst, unsigned char *buf, unsigned int size)
{
    char recBuf[10];

    spi_transmit(inst->spi_regs, buf, size, recBuf);
}

void spi_send_byte(struct esl_oled_instance *inst, unsigned char data)
{
    spi_send_buffer(inst, &data, 1);
}

static int oled_on(struct esl_oled_instance *inst)
{

    uint8_t data[10];
    uint32_t gpio;

    // powerup sequence
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    print_debug(inst->spi_regs);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // dc to low (command)
    reg_write(inst->gpio_regs, GPIO_DATA, reg_read(inst->gpio_regs, GPIO_DATA) & ~(OLED_DC));

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // sets vdd->0 res->1 vcc->0
    reg_write(inst->gpio_regs, GPIO_DATA, reg_read(inst->gpio_regs, GPIO_DATA) | OLED_VBAT | OLED_RES | OLED_VDD);

    usleep_range(1000, 1000);
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // vdd->1
    reg_write(inst->gpio_regs, GPIO_DATA, reg_read(inst->gpio_regs, GPIO_DATA) & ~(OLED_VDD));

    // res switch delay
    usleep_range(1000, 1000);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // turn off
    data[0] = 0xAE;
    spi_send_buffer(inst, data, 1);

    // set clock divide
    data[0] = 0xD5;
    data[1] = 0x80;
    spi_send_buffer(inst, data, 2);

    // set multiplex
    data[0] = 0xA8;
    data[1] = 0x1F;
    spi_send_buffer(inst, data, 2);

    // set display offset
    data[0] = 0xD3;
    data[1] = 0x00;
    spi_send_buffer(inst, data, 2);

    // set display line
    data[0] = 0x40;
    spi_send_buffer(inst, data, 1);

    // set charge pump
    data[0] = 0x8D;
    data[1] = 0x14;
    spi_send_buffer(inst, data, 2);

    // segment remap
    data[0] = 0xA0;
    spi_send_buffer(inst, data, 1);

    data[0] = 0xC0;
    spi_send_buffer(inst, data, 1);

    // com pins hardware config
    data[0] = 0xDA;
    data[1] = 0x00;
    spi_send_buffer(inst, data, 2);

    // contrast control
    data[0] = 0x81;
    data[1] = 0x7F;
    spi_send_buffer(inst, data, 2);

    // pre charge period
    data[0] = 0xD9;
    data[1] = 0xF1;
    spi_send_buffer(inst, data, 2);

    // vcomh deselect level
    data[0] = 0xDB;
    data[1] = 0x40;
    spi_send_buffer(inst, data, 2);

    // entire display on
    data[0] = 0xA4;
    spi_send_buffer(inst, data, 2);

    // normal / inverse display
    data[0] = 0xA6;
    spi_send_buffer(inst, data, 1);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // vcc->1
    reg_write(inst->gpio_regs, GPIO_DATA, reg_read(inst->gpio_regs, GPIO_DATA) & ~(OLED_VBAT));

    // 100ms
    usleep_range(100000, 100000);

    data[0] = OLED_ADDR_MODE;
    data[1] = 0x01;
    spi_send_buffer(inst, data, 2);

    data[0] = OLED_SET_COL;
    data[1] = 0x00;
    data[2] = 0x7F;
    spi_send_buffer(inst, data, 3);

    data[0] = OLED_SET_PG;
    data[1] = 0x00;
    data[2] = 0x03;
    spi_send_buffer(inst, data, 3);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    data[0] = OLED_ENABLE;
    spi_send_buffer(inst, data, 1);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);

    // turns on screen
    data[0] = 0xAE;
    spi_send_buffer(inst, data, 1);

    // turn on time
    usleep_range(100000, 100000);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    print_debug(inst->spi_regs);

    return 0;
}

// static int oled_spi_setup(struct esl_oled_instance *inst)
// {

//     // sets gpio to outputs
//     reg_write(inst->gpio_regs, GPIO_TRI, 0);
//     reg_write(inst->gpio_regs, GPIO_DATA, 0xe);

//     // contains power up sequence and hardware setup
// oled_on(inst);
//     printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);

// //     // software reset
//     reg_write(inst->spi_regs, SRR, 0xa);

//     printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);

//     // sets up SPI Control Reg
//     reg_write(inst->spi_regs, SPICR, MASTER_MODE | MASTER_INHIBIT);

//     return 0;
// }

static int spi_reset(struct esl_oled_instance *inst)
{
    unsigned int CPOL, CPHA, CLOCK;
    printk(KERN_INFO " spi pre spi reset| 0x%08X\n", reg_read(inst->spi_regs, IPISR));

    // software reset
    reg_write(inst->spi_regs, SRR, 0xa);

    printk(KERN_INFO "turn on SPE in reset 0x%08X\n", reg_read(inst->spi_regs, IPISR));

    reg_write(inst->spi_regs, SPICR, MASTER_MODE | MASTER_INHIBIT | SLAVE_ASSERT | SPE);

    // CPOL =  (1 << 4);
    // CPHA =  (1 << 3);

    // CLOCK = CPOL | CPHA;

    // reg_write(inst->spi_regs, SPICR, reg_read(inst->spi_regs, SPICR) | CLOCK);

    return 0;
}

static int oled_setup(struct esl_oled_instance *inst)
{

    uint8_t data[10];

    spi_reset(inst);

    // sets gpio to outputs
    reg_write(inst->gpio_regs, GPIO_TRI, 0);
    reg_write(inst->gpio_regs, GPIO_DATA, 0xe);

    usleep_range(1000, 1000);

    // clear vdd (set to 1)
    reg_write(inst->gpio_regs, GPIO_DATA, reg_read(inst->gpio_regs, GPIO_DATA) & ~(OLED_VDD));

    // clear res
    reg_write(inst->gpio_regs, GPIO_DATA, reg_read(inst->gpio_regs, GPIO_DATA) & ~(OLED_RES));

    usleep_range(1000, 1000);

    reg_write(inst->gpio_regs, GPIO_DATA, reg_read(inst->gpio_regs, GPIO_DATA) | (OLED_RES));

    usleep_range(1000, 1000);

    printk(KERN_ERR "turn off %s %d\n", __FUNCTION__, __LINE__);
    // turn off
    data[0] = 0xA5;
    spi_send_buffer(inst, data, 1);

    printk(KERN_ERR "clock divide %s %d\n", __FUNCTION__, __LINE__);
    // set clock divide
    data[0] = 0xD5;
    data[1] = 0x80;
    spi_send_buffer(inst, data, 2);

    printk(KERN_ERR "multiplex %s %d\n", __FUNCTION__, __LINE__);
    // set multiplex
    data[0] = 0xA8;
    data[1] = 0x1F;
    spi_send_buffer(inst, data, 2);

    printk(KERN_ERR "display offset %s %d\n", __FUNCTION__, __LINE__);
    // set display offset
    data[0] = 0xD3;
    data[1] = 0x00;
    spi_send_buffer(inst, data, 2);

    printk(KERN_ERR "display line %s %d\n", __FUNCTION__, __LINE__);
    // set display line
    data[0] = 0x40;
    spi_send_buffer(inst, data, 1);

    printk(KERN_ERR "charge pump %s %d\n", __FUNCTION__, __LINE__);
    // set charge pump
    data[0] = 0x8D;
    data[1] = 0x14;
    spi_send_buffer(inst, data, 2);

    printk(KERN_ERR "segment remap  %s %d\n", __FUNCTION__, __LINE__);
    // segment remap
    data[0] = 0xA0;
    spi_send_buffer(inst, data, 1);

    //??
    data[0] = 0xC0;
    spi_send_buffer(inst, data, 1);

    printk(KERN_ERR "com pin hardware config %s %d\n", __FUNCTION__, __LINE__);
    // com pins hardware config
    data[0] = 0xDA;
    data[1] = 0x00;
    spi_send_buffer(inst, data, 2);

    printk(KERN_ERR "contrast control %s %d\n", __FUNCTION__, __LINE__);
    // contrast control
    data[0] = 0x81;
    data[1] = 0x7F;
    spi_send_buffer(inst, data, 2);

    printk(KERN_ERR "precharge %s %d\n", __FUNCTION__, __LINE__);
    // pre charge period
    data[0] = 0xD9;
    data[1] = 0xF1;
    spi_send_buffer(inst, data, 2);

    printk(KERN_ERR "vcomh deselet %s %d\n", __FUNCTION__, __LINE__);
    // vcomh deselect level
    data[0] = 0xDB;
    data[1] = 0x40;
    spi_send_buffer(inst, data, 2);

    printk(KERN_ERR "GDDRAM fill %s %d\n", __FUNCTION__, __LINE__);
    // entire display on
    data[0] = 0xA4;
    spi_send_buffer(inst, data, 1);

    printk(KERN_ERR "normal / inverse %s %d\n", __FUNCTION__, __LINE__);
    // normal / inverse display
    data[0] = 0xA6;
    spi_send_buffer(inst, data, 1);

    // turn off
    // data[0] = 0x8D;
    // data[1] = 0x14;
    // spi_send_buffer(inst, data, 2);

    // // pre charge period
    // data[0] = 0xD9;
    // data[1] = 0xF1;
    // spi_send_buffer(inst, data, 2);

    reg_write(inst->gpio_regs, GPIO_DATA, reg_read(inst->gpio_regs, GPIO_DATA) & ~(OLED_VBAT));

    usleep_range(100000, 100000);

    data[0] = OLED_ADDR_MODE;
    data[1] = 0x01;
    spi_send_buffer(inst, data, 2);

    data[0] = OLED_SET_COL;
    data[1] = 0x00;
    data[2] = 0x7F;
    spi_send_buffer(inst, data, 3);

    data[0] = OLED_SET_PG;
    data[1] = 0x00;
    data[2] = 0x03;
    spi_send_buffer(inst, data, 3);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    data[0] = OLED_ENABLE;
    spi_send_buffer(inst, data, 1);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);

    // turns on screen
    data[0] = 0xA5;
    spi_send_buffer(inst, data, 1);

    // data[0] = 0x81;
    // data[1] = 0x0F;
    // spi_send_buffer(inst, data, 2);

    // data[0] = 0xA0;
    // spi_send_buffer(inst, data, 1);

    // data[0] = 0xC0;
    // spi_send_buffer(inst, data, 1);

    // data[0] = 0xDA;
    // data[1] = 0x00;
    // spi_send_buffer(inst, data, 2);

    // data[0] = 0xA6;
    // spi_send_buffer(inst, data, 1);

    // data[0] = 0xAF;
    // spi_send_buffer(inst, data, 1);

    // data[0] = 0xA5;
    // spi_send_buffer(inst, data, 1);

    return 0;
}

// static irqreturn_t esl_oled_irq_handler(int irq, void *dev_id)
// {
//     struct esl_oled_instance *inst = dev_id;

//     unsigned long ISR_reg;

//     // Reads the interrupt status
//     ISR_reg = ioread32(inst->spi_regs + IPISR);

//     if (ISR_reg & (1 << MODF))
//     {
//         printk(KERN_WARNING "mode error\n");
//         return -EINVAL;
//     }

//     else if (ISR_reg & (1 << SLAVE_MODF))
//     {
//         printk(KERN_WARNING "slave mode error\n");
//         return -EINVAL;
//     }

//     else if (ISR_reg & (1 << TX_UNDERRUN))
//     {
//         printk(KERN_WARNING "DTR_underrun!\n");
//         return -EINVAL;
//     }

//     else if (ISR_reg & (1 << TX_EMPTY))
//     {
//         printk(KERN_WARNING "DTR empty!\n");
//         return -EINVAL;
//     }

//     else if (ISR_reg & (1 << TX_HALF_EMPTY))
//     {
//         // if half empty wait a little to fill back up
//         printk(KERN_WARNING "tx fifo half empty!\n");
//         usleep_range(1000, 2000);
//     }

//     if (ISR_reg & (1 << RX_OVERRUN))
//     {
//         printk(KERN_WARNING "Recieve OVerrun!!!\n");
//         return -EINVAL;
//     }

//     if (ISR_reg & (1 << CPOL_CPHA))
//     {
//         printk(KERN_WARNING "CPOL_CPHA error\n");
//         return -EINVAL;
//     }

//     return IRQ_HANDLED;
// }

static int esl_oled_probe(struct platform_device *pdev)
{
    struct esl_oled_instance *inst = NULL;
    int err = 0;
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
    // sets tri state control regs to ctrl reg global
    inst->gpio_regs = ioremap(OLED_CTRL_REG, 0x128);

    if (!inst->gpio_regs)
    {
        printk(KERN_ERR "Failed to map physical address\n");
        return -ENOMEM;
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
    //     return PTR_ERR(res);
    // }

    // err = devm_request_irq(&pdev->dev, res->start,
    //                        esl_oled_irq_handler,
    //                        IRQF_TRIGGER_HIGH,
    //                        "zedoled", inst);
    // if (err < 0)
    // {
    //     return err;
    // }

    // // save irq number
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
    dev = device_create(driver_data.class, NULL, inst->devno, NULL, "zedoled%d\n", MINOR(inst->devno));
    if (IS_ERR(dev))
    {
        cdev_del(&inst->chr_dev);
        return PTR_ERR(dev);
    }

    // put into list
    INIT_LIST_HEAD(&inst->inst_list);
    list_add(&inst->inst_list, &driver_data.instance_list);

    // ooled_setupled_spi_setup(inst);

    oled_setup(inst);

    return 0;
}

static int esl_oled_remove(struct platform_device *pdev)
{
    struct esl_oled_instance *inst = platform_get_drvdata(pdev);
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);

    // oled_off(inst);

    iounmap(inst->gpio_regs);
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    cdev_del(&inst->chr_dev);
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // cleanup and remove
    device_destroy(driver_data.class, inst->devno);
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // remove from list
    list_del(&inst->inst_list);

    return 0;
}

static struct of_device_id esl_oled_of_ids[] = {
    {.compatible = "esl,null-spi"},
    {}};

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
    // plat driver unregister
    platform_driver_unregister(&esl_oled_driver);
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);

    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // free character device region
    unregister_chrdev_region(driver_data.first_devno, 1);
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);
    // remove class
    class_destroy(driver_data.class);
    printk(KERN_ERR "I am her %s %d\n", __FUNCTION__, __LINE__);

    printk(KERN_INFO "Sucessfully exited OLED module\n");
}

module_init(esl_oled_init);
module_exit(esl_oled_exit);

MODULE_DESCRIPTION("ZedBoard OLED driver");
MODULE_LICENSE("GPL");
