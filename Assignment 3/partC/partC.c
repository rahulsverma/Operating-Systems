
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>

static const char    hello_world_str[] = "Hello world from kernel module!\n\0";
static const ssize_t hello_world_str_size = sizeof(hello_world_str);

static int sample_open(struct inode *inode, struct file *file)
{
    pr_info("I have been awoken\n");
    return 0;
}

// Added: 1
static ssize_t sample_read(struct file *file, char __user *user_buffer, size_t length, loff_t *offset)
{
    if( *offset >= hello_world_str_size )
        return 0;

    /* If a user tries to read more than we have, read only as many bytes as we have */

    if( *offset + length > hello_world_str_size )
       length = hello_world_str_size - *offset;

    if( copy_to_user(user_buffer, hello_world_str + *offset, length) != 0 )
        return -EFAULT;

    /* Move reading offset */
    *offset += length;

    return length;
}

static int sample_close(struct inode *inodep, struct file *filp)
{
    pr_info("Sleepy time\n");
    return 0;
}

static ssize_t sample_write(struct file *file, const char __user *buf,
                       size_t len, loff_t *ppos)
{
    pr_info("Yummy - I just ate %d bytes\n", len);
    //pr_info("%s", buf);
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
    .name = "sudeep_rahul",
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

    pr_info("I'm in\n");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&sample_device);
    pr_info("I'm out\n");
}

module_init(misc_init)
module_exit(misc_exit)

MODULE_DESCRIPTION("Simple Misc Driver");
MODULE_AUTHOR("Nick Glynn <n.s.glynn@gmail.com>");
MODULE_LICENSE("GPL");
