#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "chardev"
#define BUF_LEN 80

static int major;
static char msg[BUF_LEN];
static struct cdev my_cdev;

static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char *buffer, size_t len, loff_t *offset) {
    int bytes_read = 0;
    while (len && msg[bytes_read] != 0) {
        put_user(msg[bytes_read++], buffer++);
        len--;
    }
    return bytes_read;
}

static ssize_t dev_write(struct file *file, const char *buffer, size_t len, loff_t *offset) {
    int i;
    for (i = 0; i < len && i < BUF_LEN; i++) {
        get_user(msg[i], buffer + i);
    }
    return i;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init chardev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register character device\n");
        return major;
    }
    printk(KERN_INFO "Registered char device with major number %d\n", major);
    return 0;
}

static void __exit chardev_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Unregistered char device\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver");
