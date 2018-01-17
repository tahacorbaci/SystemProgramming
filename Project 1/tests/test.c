/*
BLG 413E  System Programming
Homework 1
Mehmet Taha Çorbacıoğlu - 150130017
Kağan Özgün - 150130055
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <signal.h>
#include <string.h>

int main(){
    int Flag;
    int child1, child2, child3, parent;
    int error;
    int i;
    parent = getpid();  //Parent PID
    char str[10];
    sprintf(str, "%d", parent);         //Parent PID to string            

    child1 = fork();    //Parent's first child is created.
    if(child1==0){      //First Child Process
        sleep(10);
        printf("This is a child1 process and pid is %d\nParent pid: %d\n",getpid(),getppid());
        scanf("%d",&i);       
    }
    else{               //Parent process continues
        sleep(1);
        child2=fork();
        if(child2==0){  //Parent's second child is created.
            sleep(10);
            printf("This is child2  process and pid is %d\nParent pid: %d\n",getpid(),getppid());
            scanf("%d",&i);          
        }
        else{           //Parent process continues
            printf("The parent process has pid %d\n",parent);
            printf("Child1 pid: %d\n", child1);
            printf("Child2 pid: %d\n", child2);
            printf("Enter Flag: ");     //Entering Parent flag value
            scanf("%d", &Flag);
        
            error = syscall(355,parent,Flag); //Setting Parent's flag value
            printf("System call returns: %d\n", error);     //If error occurs it returns -1 otherwise 0
            fprintf(stderr,"value of errno: %d\n",errno);   //Prints error codes 
            perror("Error printed by perror");
            /*
            ESRCH            3      No such process
            EACCES          13      Permission denied
            EBADMSG         74      Not a data message
            */ 
            
            printf("Do you want to renice process.(Yes:1, No:0)"); //Set nice value of parent process 11 to make greater than 10.
            scanf("%d", &i);
            if(i ==1){
                char renice[50] = "renice 11 -p ";  //Terminal command
                strcat(renice, str);                //Adds Parent PID to end of renice string.
                system(renice);                     //Call terminal command
            }
            else{
                char nice0[50] = "renice 0 -p ";    //Terminal command to ensure that parent has 0 nice value.
                strcat(nice0, str);                //Adds Parent PID to end of nice0 string.
                system(nice0);                      //Call terminal command.
            }
            wait(NULL);
            scanf("%d", &i);
        }
    }
    return 0;
}