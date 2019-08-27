#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
static struct workqueue_struct *queue=NULL;
static struct work_struct   work;

static void work_handler(struct work_struct *data)
{

       printk(KERN_ALERT"work handler function.==%x\n",data);
	    msleep(500);
}

static int __init test_init(void)
{
      queue=create_singlethread_workqueue("hello world");/*创建一个单线程的工作队列*/
      if (!queue)
            goto err;

       INIT_WORK(&work,work_handler);
       //schedule_work(&work);
	   queue_work(queue, &work);
	   printk(KERN_ALERT"work_struct :%p",work);

      return 0;
err:
      return-1;
}

static   void __exit test_exit(void)
{
   destroy_workqueue(queue);
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("myserial init");