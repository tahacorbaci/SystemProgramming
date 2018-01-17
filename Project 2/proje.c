/*Kağan Özgün 150130055
 * Mehmet Taha Çorbacıoğlu 150130017*/
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	
#include <linux/slab.h>		
#include <linux/fs.h>		
#include <linux/errno.h>	
#include <linux/types.h>	
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/sched.h>

#include <asm/switch_to.h>		
#include <asm/uaccess.h>	


#define SCULL_MAJOR 0
#define SCULL_NR_DEVS 4
#define SCULL_QUANTUM 4000
#define SCULL_QSET 1000

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET;

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

MODULE_AUTHOR("Kagan Ozgun, Mehmet Taha Corbacioglu");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev {
    char **data;
    int quantum;
    int qset;
    unsigned long size;
    struct semaphore sem;
    struct cdev cdev;
};

struct scull_dev *scull_devices;


int scull_trim(struct scull_dev *dev)
{
    int i;

    if (dev->data) {
        for (i = 0; i < dev->qset; i++) {
            if (dev->data[i])
                kfree(dev->data[i]);
        }
        kfree(dev->data);
    }
    dev->data = NULL;
    dev->quantum = scull_quantum;
    dev->qset = scull_qset;
    dev->size = 0;
    return 0;
}


int scull_open(struct inode *inode, struct file *filp)
{
    struct scull_dev *dev;

    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev;

    
    return 0;
}


int scull_release(struct inode *inode, struct file *filp)
{
    return 0;
}
char *CurUser;
char* find_current_user(void){                                             //function for takim current username from /etc/passwd depend of uid
	
    
    printk(KERN_INFO "yey\n");
	struct file *f;
    char *tmp;
    mm_segment_t fs;
    
    printk(KERN_INFO "My module is loaded\n");
    
    f = filp_open("/etc/passwd", O_RDONLY, 0);                  // openfile
   
	
	fs = get_fs();
	set_fs(get_ds());
	
	tmp = kmalloc(sizeof(char*), GFP_KERNEL);                   // read all character in file one by one for counting the number of char to detect end of file
	int h;
	int read;
	for(h=0;;h++){
		read = f->f_op->read(f, tmp,1, &f->f_pos);
		if(read== 0)
			break;
	}
	set_fs(fs);
	filp_close(f,NULL);                                      // close an dopen file for read from begining
	f = filp_open("/etc/passwd", O_RDONLY, 0);
	fs = get_fs();
	set_fs(get_ds());
	char *passwd;
	char *CurId	;
	char *R_Word;
	passwd = kmalloc((h-1)*sizeof(char),GFP_KERNEL);              // passwd array took all character in file
	f->f_op->read(f, passwd,h, &f->f_pos);
	char *rLine;
	printk(KERN_INFO "out of first while\n");
	while ((rLine = strsep(&passwd, "\n")) != NULL){                      // check lines depend on \n
		printk(KERN_INFO "first while\n");
		int kgn =0;
        while ((R_Word = strsep(&rLine, ":")) != NULL){                 // separate words with :
			printk(KERN_INFO "second while\n");
            if(kgn == 0){                                            // if counter 0 that word username
                CurUser = kmalloc(33*sizeof(char),GFP_KERNEL);
                CurUser[0] = '\0';
                strcat(CurUser,R_Word);
                kgn++;
            }else if(kgn == 2){
				CurId = kmalloc(8*sizeof(char),GFP_KERNEL);               //if counter 2 that word is uid
                CurId[0] = '\0';
                strcat(CurId,R_Word);
                kgn++;
			}else if( kgn > 2){                        // if counter greater than 2 cast uid to long and compare with curen uid
				long uId;
				kstrtol(CurId,10,&uId);
				if(uId == get_current_cred()->uid.val){          // if equal return username
					set_fs(fs);
					filp_close(f,NULL);
					kfree(passwd);
					kfree(CurId);
					return CurUser;
				}
			}else{
				kgn++;
			}
		}
            
    }
    return NULL;
}
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos)
{
    struct scull_dev *dev = filp->private_data;
    int quantum = dev->quantum;
    int s_pos, q_pos;
    ssize_t retval = 0;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    if (*f_pos >= dev->size)
        goto out;
    if (*f_pos + count > dev->size)
        count = dev->size - *f_pos;

    s_pos = (long) *f_pos / quantum;
    q_pos = (long) *f_pos % quantum;

    if (dev->data == NULL || ! dev->data[s_pos])
        goto out;

    if (count > quantum - q_pos)
        count = quantum - q_pos;

    //Read usernames
    char *backup = kmalloc(strlen(dev->data[s_pos]) * sizeof(char), GFP_KERNEL);			//it creates a copy of the data in file to protect the data in file.
    strcpy(backup, dev->data[s_pos]);
    char *messageList = kmalloc(strlen(dev->data[s_pos]) * sizeof(char), GFP_KERNEL);		//creates a message list to store messages that sent to user
    messageList[0] = '\0';
    char *message;
    char *line;
    char *word;
    char *username;
    int i = 0;
    char *uname = kmalloc(strlen(find_current_user()) * sizeof(char), GFP_KERNEL);			//Uname stores current user's name
    strcpy(uname, find_current_user());
    kfree(CurUser);
    printk(KERN_WARNING "uname: %s\n", uname);
    while ((line = strsep(&backup, "\n")) != NULL){											//it takes line by line from backup
        message = kmalloc(strlen(line) * sizeof(char), GFP_KERNEL);
        message[0] = '\0';
        while ((word = strsep(&line, " ")) != NULL){										//it takes word by word from line 
            if(word[0] == '@'){																//word's first letter is @ then it means it is user.
                username = kmalloc(strlen(word) * sizeof(char), GFP_KERNEL);				//username stores usernames that is in the file and difference with word is @
                for(i = 1; i < strlen(word); i++){
                    username[i-1] = word[i];
                }
                username[strlen(word)-1] = '\0';
                //current_uid().val
                printk(KERN_WARNING "username: %s \n", username);
            }
            else{
                if((word[0] < 127 && word[0] > 32 && word[0] !=92)){						//if word is meaningful then it adds to message. The reason of that messages can includes more than one word.
                    strcat(message, word);
                    strcat(message, " ");
                }                
            }
        }     
        if(message[0] != ' ' && message[0] != '\0' && message[0] < 127 && message[0] > 32 && strcmp(username,uname) == 0 ){		//if username in the file is equal current username than messages is added to message list
            strcat(messageList, message);
            strcat(messageList, "\n");
            //printk(KERN_WARNING "message: %s len: %d\n", message, strlen(message));
        }
        kfree(username);   
        kfree(message);
    }
	printk(KERN_WARNING "%s\n", uname);
    kfree(uname);
    
    //dev->data[s_pos] + q_pos
    if (copy_to_user(buf, messageList, strlen(messageList))) {
        retval = -EFAULT;
        goto out;
    }
    kfree(line);
    kfree(word);
    kfree(backup);
    kfree(messageList);
    *f_pos += count;
    retval = count;
	printk(KERN_WARNING "end of read \n");
  out:
    up(&dev->sem);
    return retval;
}


ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos)
{
    struct scull_dev *dev = filp->private_data;
    int quantum = dev->quantum, qset = dev->qset;
    int s_pos, q_pos;
    ssize_t retval = -ENOMEM;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if (*f_pos >= quantum * qset) {
        retval = 0;
        goto out;
    }

    s_pos = (long) *f_pos / quantum;
    q_pos = (long) *f_pos % quantum;

    if (!dev->data) {
        dev->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
        if (!dev->data)
            goto out;
        memset(dev->data, 0, qset * sizeof(char *));
    }
    if (!dev->data[s_pos]) {
        dev->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
        if (!dev->data[s_pos])
            goto out;
    }
    if (count > quantum - q_pos)
        count = quantum - q_pos;

    if (copy_from_user(dev->data[0] + dev->size, buf, count)) {		//it writes to file by looking its size.
        retval = -EFAULT;
        goto out;
    }
    
    *f_pos += count;  
    retval = count;
    dev->size += *f_pos;

  out:
    up(&dev->sem);
    return retval;
}


loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
    struct scull_dev *dev = filp->private_data;
    loff_t newpos;

    switch(whence) {
        case 0: /* SEEK_SET */
            newpos = off;
            break;

        case 1: /* SEEK_CUR */
            newpos = filp->f_pos + off;
            break;

        case 2: /* SEEK_END */
            newpos = dev->size + off;
            break;

        default: /* can't happen */
            return -EINVAL;
    }
    if (newpos < 0)
        return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}


struct file_operations scull_fops = {
    .owner =    THIS_MODULE,
    .llseek =   scull_llseek,
    .read =     scull_read,
    .write =    scull_write,
    .open =     scull_open,
    .release =  scull_release,
};


void scull_cleanup_module(void)
{
    int i;
    dev_t devno = MKDEV(scull_major, scull_minor);

    if (scull_devices) {
        for (i = 0; i < scull_nr_devs; i++) {
            scull_trim(scull_devices + i);
            cdev_del(&scull_devices[i].cdev);
        }
    kfree(scull_devices);
    }

    unregister_chrdev_region(devno, scull_nr_devs);
}


int scull_init_module(void)
{
    int result, i;
    int err;
    dev_t devno = 0;
    struct scull_dev *dev;

    if (scull_major) {
        devno = MKDEV(scull_major, scull_minor);
        result = register_chrdev_region(devno, scull_nr_devs, "scull");
    } else {
        result = alloc_chrdev_region(&devno, scull_minor, scull_nr_devs,
                                     "scull");
        scull_major = MAJOR(devno);
    }
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }

    scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev),
                            GFP_KERNEL);
    if (!scull_devices) {
        result = -ENOMEM;
        goto fail;
    }
    memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

   
    for (i = 0; i < scull_nr_devs; i++) {
        dev = &scull_devices[i];
        dev->quantum = scull_quantum;
        dev->qset = scull_qset;
        sema_init(&dev->sem,1);
        devno = MKDEV(scull_major, scull_minor + i);
        cdev_init(&dev->cdev, &scull_fops);
        dev->cdev.owner = THIS_MODULE;
        dev->cdev.ops = &scull_fops;
        err = cdev_add(&dev->cdev, devno, 1);
        if (err)
            printk(KERN_NOTICE "Error %d adding scull%d", err, i);
    }

    return 0; 

  fail:
    scull_cleanup_module();
    return result;
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
