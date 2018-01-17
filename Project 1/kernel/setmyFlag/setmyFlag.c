#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/errno.h>

asmlinkage long set_myFlag(pid_t pid, int flag){
    struct task_struct *t;
    
     if(current_uid() != 0){
        return -EACCES;
     }
        
    t = find_task_by_vpid(pid);
    if(t == NULL)
        return -ESRCH;
    if(flag == 0 || flag == 1)
        t->myFlag = flag;  
    else
        return -EBADMSG;
         
    return 0;
}
