#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>

#include <linux/sched/signal.h>

#include "process.h"


//linux structure defined in sched.h
static struct task_struct *plist;

// defined in process.h
// array of process_info structure
// holds task_struct data returned from kernel module
struct process_info pinfo[MAX_BUFFER];


/*
pinfo[0].pid =plist->pid
pinfo[1].pid
pinfo[2].pid
*/

static int sample_open(struct inode *inode, struct file *file)
{
    return 0;
}

// when user calls read() on /dev/process_list
// in userspace this function gets executed in kernel space

// userspace --> read(u_buffer)---->kernel--->fill(k_buffer)-->
// copy_to_user(k_buffer, u_buffer) --> userspace()


static ssize_t sample_read(struct file *file, char __user *user_buffer, size_t length, loff_t *offset)
{
    int error_count =0;
    int cnt = 0;


    for_each_process(plist)
    {

       pinfo[cnt].pid=plist->pid;
       pinfo[cnt].ppid=plist->parent->pid;
       pinfo[cnt].state = plist->state;
       pinfo[cnt].cpu = task_cpu(plist);

       cnt++;
    }
    error_count = copy_to_user((struct process_info*)user_buffer, pinfo, MAX_BUFFER);


    if (error_count != 0){
                printk(KERN_INFO "kernel: Failed to send %d characters to the user\n", error_count);
                return -EFAULT;
        }
        return cnt;
}

static int sample_close(struct inode *inodep, struct file *filp)
{
    return 0;
}

static ssize_t sample_write(struct file *file, const char __user *buf,
                       size_t len, loff_t *ppos)
{
    return len; /* But we don't actually do anything with the data */
}

static const struct file_operations sample_fops = {
    .owner                      = THIS_MODULE,

// Added 2
    .read                       = sample_read,

    .write                      = sample_write,
    .open                       = sample_open,
    .release            = sample_close,
    .llseek             = no_llseek,
};

struct miscdevice sample_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "process_list",
    .fops = &sample_fops,
};

static int __init misc_init(void)
{
    int error;

    error = misc_register(&sample_device);
    if (error) {
        pr_err("can't misc_register :(\n");
        return error;
    }

    pr_info("Module registered\n");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&sample_device);
    pr_info("Module removed\n");
}

module_init(misc_init)
module_exit(misc_exit)

MODULE_DESCRIPTION("Process List Driver");
MODULE_AUTHOR("Rahul & Sudeep");
MODULE_LICENSE("GPL");
