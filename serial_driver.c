#include <linux/module.h>
#include <linux/tty.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include <linux/tty_flip.h>
#include <linux/serial_reg.h>
#include <linux/serial.h>
#include <linux/serial_core.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/irq.h>
#include <asm/bitops.h>
#include <asm/byteorder.h>
#include <asm/serial.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "linux/version.h"
#include <linux/platform_device.h>

#define SW_UART_NR	4

#define is_real_interrupt(irq)	((irq) != 0)


static struct timer_list *timer;
volatile static int force_enable_work;
struct work_struct q_work;
struct workqueue_struct *workqueue;

static DEFINE_MUTEX(serial_mutex);


#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 8, 0)
#define __devinitdata
#define __devinit
#define __devexit
#define __devexit_p
#endif



//当有硬件数据到来时，要手动调用该函数接收数据并上报
static void sun_uart_handle_rx(unsigned long data)
{
#if 0
    struct uart_port * port;
    struct tty_struct *tty;
    //unsigned char ch = 0;
    //int i;
    int flag;
    unsigned char *send_buff = "i am sclu, this is from serial rx";
 
    flag = TTY_NORMAL;
    printk("%d,%s\n", __LINE__, __func__);
    port = (struct uart_port *)data;
    if (!port)
        return;
    if (!port->state)
        return;
    tty = port->state->port.tty;
    //tty->flags |= TTY_HW_COOK_IN;
    //tty = port->info->tty;
    if (!tty)
        return;
 
  //  for (i = 0; i < strlen(send_buff) + 1; i++) {
        //用tty_insert_flip_char函数也可以
    //    ch = send_buff[i];
      //  uart_insert_char(port, 0, 0, ch, flag);
   // }
   tty_insert_flip_string(tty, send_buff, (strlen(send_buff) + 1));
//    uart_insert_char(port, 0, 0, '\n', flag);
  //  uart_insert_char(port, 0, 0, '\t', 0);
    tty_flip_buffer_push(tty);

      if(timer !=NULL)
      	{
	    timer->expires = jiffies + 3 * HZ;
	    add_timer(timer);
      	}
	#endif
}
 
static void sun_uart_stop_tx(struct uart_port *port)
{
    //del_timer(timer);
   printk("%d,%s\n", __LINE__, __func__);
}
 
//上层用户空间写的数据最终调用该函数完成发送
static void sun_uart_start_tx(struct uart_port *port)
{
    struct circ_buf *xmit = &port->state->xmit;
    printk(" %d, %s\n",__LINE__, __func__);
    //开始发送数据
    if(!force_enable_work || uart_circ_empty(xmit) || uart_tx_stopped(port))
        queue_work(workqueue, &q_work);
}
 
 
 
static unsigned int sun_uart_tx_empty(struct uart_port *port)
{
    printk("%d,%s\n", __LINE__, __func__);
    //当硬件fifo中的数据发送完成后，要返回TIOCSER_TEMI，否则上层一直写等待，如echo数据一直没有返回
    return TIOCSER_TEMT;
}
 
static void sun_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
   printk("%d,%s\n", __LINE__, __func__);
}
 
static unsigned int sun_uart_get_mctrl(struct uart_port *port)
{
    printk("%d,%s\n", __LINE__, __func__);
    return 0;
}
 
static void sun_uart_stop_rx(struct uart_port *port)
{
    printk("%d,%s\n", __LINE__, __func__);
}
 
static void sun_uart_enable_ms(struct uart_port *port)
{
   printk("%d,%s\n", __LINE__, __func__);
}
 
static void sun_uart_break_ctl(struct uart_port *port, int break_state)
{
	unsigned long flags;
   // spin_lock_irqsave(&up->port.lock, flags);
   printk("%d,%s\n", __LINE__, __func__);
   // spin_unlock_irqrestore(&up->port.lock, flags);
}
 
static int sun_uart_startup(struct uart_port *port)
{
	int retval;
	unsigned long flags;

    /* this is the first time this port is opened */
    /* do any hardware initialization needed here */
 
    /* create our timer and submit it */
    force_enable_work = 0;  
	printk("%d, %s,irq:%dtimer:%p\n", __LINE__, __func__,port->irq,timer);

	/*if(port->irq) {
		retval = serial_link_irq_chain(up);
		if(retval)
			return retval;
	}*/
	//spin_lock_irqsave(&port->lock, flags);
		
	/*
	* Most PC uarts need OUT2 raised to enable interrupts.
	*/
	//if (is_real_interrupt(port->irq))
	//	port->mctrl |= TIOCM_OUT2;
	//to enable intenal loop, uncomment the line below
	//up->port.mctrl |= TIOCM_LOOP;

	//spin_unlock_irqrestore(&port->lock, flags);
	
   #if 0
    if (!timer) {
        timer = kmalloc(sizeof(*timer), GFP_KERNEL);
        if (!timer)
            return -ENOMEM;
    }   
    printk("%d, %s,irq:%dtimer:%p\n", __LINE__, __func__,port->irq,timer);
    setup_timer(timer, sun_uart_handle_rx, (unsigned long)port);
    timer->expires = jiffies + HZ * 3;
    #if 0   
    timer->data = (unsigned long)port;
    timer->expires = jiffies + DELAY_TIME;
    timer->function = tiny_timer;
    #endif  
    add_timer(timer);
  #endif
  
    return 0;
}
 
static void sun_uart_shutdown(struct uart_port *port)
{
	unsigned long flags;

    /* The port is being closed by the last user. */
    /* Do any hardware specific stuff here */
 
    /* shut down our timer */
    printk("%d,%s\n", __LINE__, __func__);
    force_enable_work = 1;
    cancel_work_sync(&q_work);
	//spin_lock_irqsave(&port->lock, flags);
	//port->mctrl &= ~TIOCM_OUT2;
	//spin_unlock_irqrestore(&port->lock, flags);

  
}
 
static void sun_uart_set_termios(struct uart_port *port, struct ktermios *termios,
        struct ktermios *old)
{
	unsigned char cval;
	unsigned long flags;
	signed int baud;
    printk("%d,%s\n", __LINE__, __func__);

	 int port_index = port->line;
	switch (termios->c_cflag & CSIZE) 
	{
	case CS5:
			cval = 0x00;
			break;
	case CS6:
			cval = 0x01;
			break;
	case CS7:
			cval = 0x02;
			break;
	default:
	case CS8:
			cval = 0x03;
			break;
	}

	if (termios->c_cflag & CSTOPB)
			cval |= 0x04;
	if (termios->c_cflag & PARENB)
			cval |= UART_LCR_PARITY;
	if (!(termios->c_cflag & PARODD))
			cval |= UART_LCR_EPAR;
		#ifdef CMSPAR
		if (termios->c_cflag & CMSPAR)
			cval |= UART_LCR_SPAR;
		#endif
	//baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk/4);
	printk(KERN_INFO "\nserialxr_set_termios: Port Index:%d c_ispeed:%d c_ospeed:%d baud=%d,%x\r\n",port_index,termios->c_ispeed,termios->c_ospeed,baud,port->uartclk);
     /*
	 * Ok, we're now changing the port state.  Do it with
	 * interrupts disabled.
	 */

	 #if 0
	spin_lock_irqsave(&port->lock, flags);
	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	port->read_status_mask = UART_LSR_OE | UART_LSR_THRE | UART_LSR_PE | UART_LSR_DR;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= UART_LSR_FE | UART_LSR_PE;
	if (termios->c_iflag & (BRKINT | PARMRK))
		port->read_status_mask |= UART_LSR_BI;
	/*
	 * Characteres to ignore
	 */
	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= UART_LSR_PE | UART_LSR_FE;
	if (termios->c_iflag & IGNBRK) {
		port->ignore_status_mask |= UART_LSR_BI;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			port->ignore_status_mask |= UART_LSR_OE;
	}

	/*
	 * ignore all characters if CREAD is not set
	 */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |= UART_LSR_DR;
	/*
	 * CTS flow control flag and modem status interrupts
	 */
	 UART_ENABLE_MS(port, termios->c_cflag);
	 spin_unlock_irqrestore(&port->lock, flags);
	 #endif


}
 
static const char *sun_uart_type(struct uart_port *port)
{
    printk("%d,%s\n", __LINE__, __func__);
    return "SZG";
}
 
static void sun_uart_release_port(struct uart_port *port)
{
    printk("%d,%s\n", __LINE__, __func__);
}
 
static int sun_uart_request_port(struct uart_port *port)
{
    printk("%d,%s\n", __LINE__, __func__);
    return 0;
}
 
static void sun_uart_config_port(struct uart_port *port, int flags)
{
     printk("%d,%s,%d\n", __LINE__, __func__,flags);
}
 
static int sun_uart_verify_port(struct uart_port *port, struct serial_struct *ser)
{
    printk("%d,%s\n", __LINE__, __func__);
    return 0;
}
 
static void sun_uart_pm(struct uart_port *port, unsigned int state,
        unsigned int oldstate)
{
     printk("%d, %s,state:%d,oldstate:%d\n", __LINE__, __func__,state,oldstate);
 
}

static struct uart_driver sunzhguy_uart_driver = {
	.owner = THIS_MODULE,
	.driver_name = "sunzhguy_serial",
	.dev_name = "ttySZ",
	.nr = SW_UART_NR,
	.cons = NULL,
};
		 



static struct uart_ops sunzhguy_uart_ops = {
    .tx_empty = sun_uart_tx_empty,
    .set_mctrl = sun_uart_set_mctrl,
    .get_mctrl = sun_uart_get_mctrl,
    .stop_tx = sun_uart_stop_tx,
    .start_tx = sun_uart_start_tx,
    .stop_rx = sun_uart_stop_rx,
    .enable_ms = sun_uart_enable_ms,
    .break_ctl = sun_uart_break_ctl,
    .startup = sun_uart_startup,
    .shutdown = sun_uart_shutdown,
    .set_termios = sun_uart_set_termios,
    .pm = sun_uart_pm,
    .type = sun_uart_type,
    .release_port = sun_uart_release_port,
    .request_port = sun_uart_request_port,
    .config_port = sun_uart_config_port,
//    .verify_port = sw_uart_verify_port,
};

static struct uart_port sunzhguy_port = {
    .ops = &sunzhguy_uart_ops,
    .type = PORT_8250,
    .fifosize = 256,
    .flags = UPF_SKIP_TEST | UPF_BOOT_AUTOCONF,
};
static void tx_work(struct work_struct *work)
{
	unsigned long flags;

#if 1
		struct uart_port *port = &sunzhguy_port;
		struct circ_buf *xmit = &port->state->xmit;
		int count;	 
		 printk("%d, %s\n", __LINE__, __func__);
		 spin_lock_irqsave(&port->lock, flags);
		 
		if (port->x_char) {
			printk("x_char %2x", port->x_char);
			port->icount.tx++;
			port->x_char = 0;
			goto return_break;
		}
			 
		if (uart_circ_empty(xmit))
		{
          //serialxr_stop_tx(port);
		  goto return_break;
		}
		if(uart_tx_stopped(port)) {
		  goto return_break;
		}
			 
		count = port->fifosize / 2;
	    printk("sclu %s, count = %d,tail :%d\n", __func__, count,xmit->tail);

		if (uart_circ_chars_pending(xmit) < count)
		count = uart_circ_chars_pending(xmit);
		//发送数据
		//	serial_out(&sw_uport->port, xmit->buf[xmit->tail], SW_UART_THR);
		printk("get data:\n");
		do {
			printk("%c", xmit->buf[xmit->tail]);
			xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
			port->icount.tx++;
			if (uart_circ_empty(xmit)) {
				printk("<empty>\n");
				goto return_break;
			  }
			} while (--count > 0);

				printk("<>\n");
			if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
			  uart_write_wakeup(port);
			  printk("<wakeup>\n");
			}
		   if (uart_circ_empty(xmit))
			 sun_uart_stop_tx(port);
#endif
return_break:
spin_unlock_irqrestore(&port->lock, flags);

			 
}


#if 0
static int __init sunzhguy_uart_init(void)
{
    int ret;
	unsigned long flags;
    printk("%d, %s\n", __LINE__, __func__);
    workqueue = create_singlethread_workqueue("sunzhguySerial_work");
    INIT_WORK(&q_work, tx_work);
    ret = uart_register_driver(&sunzhguy_uart_driver);
    if (unlikely(ret)) {
        printk(KERN_WARNING"serial driver initializied err\n");
        return ret;
    }
	mutex_lock(&serial_mutex);
	printk(KERN_WARNING"-------lock1:%x,%p\n",sunzhguy_port.lock,&sunzhguy_port.lock);

	printk("ret====<<>>======%d,%d---->\r\n",ret,sunzhguy_uart_driver.nr,sunzhguy_port.line);
    ret= uart_add_one_port(&sunzhguy_uart_driver, &sunzhguy_port);
	spin_lock_init(&sunzhguy_port.lock);
	spin_lock_irqsave(&sunzhguy_port.lock, flags);
	
	printk(KERN_WARNING"-------lock2:%x,%d\n",sunzhguy_port.lock,flags);
	spin_unlock_irqrestore(&sunzhguy_port.lock, flags);
	printk(KERN_WARNING"-------lock3:%x,%d\n",sunzhguy_port.lock,flags);
	mutex_unlock(&serial_mutex);
	return ret;
}


static void __exit sunzhguy_uart_exit(void)
{
   if (timer) {
        del_timer(timer);
        timer = NULL;
    }
    if (workqueue) {
        flush_workqueue(workqueue);
        destroy_workqueue(workqueue);
        workqueue = NULL;
    }
    printk("driver exit\n");
	mutex_lock(&serial_mutex);
    uart_remove_one_port(&sunzhguy_uart_driver, &sunzhguy_port);
	mutex_unlock(&serial_mutex);
    //platform_driver_unregister(&sw_uport_platform_driver);
    //if (pdev)
    //platform_device_unregister(pdev);
   //uart_remove_one_port(&sw_uart_driver, &tiny_port);
    uart_unregister_driver(&sunzhguy_uart_driver);
}
#endif

static int __devinit init_one_sunpciserialcard(struct pci_dev *dev, const struct pci_device_id *ent)
{

	int ret;
	unsigned long flags;
    printk("%d, %s\n", __LINE__, __func__);
    workqueue = create_singlethread_workqueue("sunzhguySerial_work");
    INIT_WORK(&q_work, tx_work);
	#if 0
    ret = uart_register_driver(&sunzhguy_uart_driver);
    if (unlikely(ret)) {
        printk(KERN_WARNING"serial driver initializied err\n");
        return ret;
    }
	#endif
	mutex_lock(&serial_mutex);
	printk(KERN_WARNING"-------lock1:%x,%p\n",sunzhguy_port.lock,&sunzhguy_port.lock);

	printk("ret====<<>>======%d,%d---->\r\n",ret,sunzhguy_uart_driver.nr,sunzhguy_port.line);
    ret= uart_add_one_port(&sunzhguy_uart_driver, &sunzhguy_port);
	spin_lock_init(&sunzhguy_port.lock);
	spin_lock_irqsave(&sunzhguy_port.lock, flags);
	
	printk(KERN_WARNING"-------lock2:%x,%d\n",sunzhguy_port.lock,flags);
	spin_unlock_irqrestore(&sunzhguy_port.lock, flags);
	printk(KERN_WARNING"-------lock3:%x,%d\n",sunzhguy_port.lock,flags);
	mutex_unlock(&serial_mutex);
	return ret;
    
}

static void __devexit remove_one_sunpciserialcard(struct pci_dev *dev)
{
	struct serial_private *priv = pci_get_drvdata(dev);
#if 0
	pci_set_drvdata(dev, NULL);

	pciserial_remove_ports(priv);
#endif
	uart_unregister_driver(&sunzhguy_uart_driver);
	pci_disable_device(dev);
}



static struct pci_device_id sunserial_pci_tbl[] = {
	{	0x8086, 0x7110,
		PCI_ANY_ID, PCI_ANY_ID,
		0, 0, 0 },
	{ 0, }
};





static struct pci_driver sunserial_pci_driver = {
	.name		= "sunzhguyserial",
	.probe		= init_one_sunpciserialcard,
	.remove		= __devexit_p(remove_one_sunpciserialcard),
	.id_table	= sunserial_pci_tbl,
};


static int __init serialsun_init(void)
{
	int ret;

	printk(KERN_INFO "PCIE  ----------------Serial  Driver...................\r\n");

	ret = uart_register_driver(&sunzhguy_uart_driver);
	if (ret)
		return ret;

	ret = pci_register_driver(&sunserial_pci_driver);

	if (ret < 0)
		uart_unregister_driver(&sunzhguy_uart_driver);

	return ret;	
}

static void __exit serialsun_exit(void)
{
	pci_unregister_driver(&sunserial_pci_driver);
	uart_unregister_driver(&sunzhguy_uart_driver);
}

//module_init(serialxr_init);
//module_exit(serialxr_exit);

///MODULE_LICENSE("GPL");
//MODULE_DESCRIPTION("myPCIE init");


//module_init(sunzhguy_uart_init);
//module_exit(sunzhguy_uart_exit);

module_init(serialsun_init);
module_exit(serialsun_exit);

 
MODULE_AUTHOR("ztzh@sunzhg20@qq.com>");
MODULE_DESCRIPTION("Driver for Sunzhguy serial device");
MODULE_LICENSE("GPL");




