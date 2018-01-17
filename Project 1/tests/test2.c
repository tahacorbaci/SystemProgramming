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
    int parent, child1, child2;
    char Parent[10];
    char Child1[10];
    char Child2[10];
    int i;
    printf("Enter parent PID: ");
    scanf("%d", &parent);

    printf("Enter child1 PID: ");
    scanf("%d", &child1);
    printf("Enter child2 PID: ");
    scanf("%d", &child2);

    char command[20] = "pstree -p ";    //Terminal command to show as a tree
    sprintf(Parent, "%d", parent);      //Parent PID to string
    strcat(command, Parent);            //Adds Parent PID to end of command string.
    system(command);                    //Call terminal command

    char ps[20] = "ps ";                //Terminal command to show all process(parent, child1, child2)
    
    strcat(ps, Parent);                 //Adds Parent PID to end of ps string.
    strcat(ps," ");
    sprintf(Child1, "%d", child1);      //Child1 PID to string
    strcat(ps, Child1);                 //Adds Child1 PID to end of ps string.
    strcat(ps," ");
    sprintf(Child2, "%d", child2);      //Child2 PID to string
    strcat(ps, Child2);                 //Adds Child2 PID to end of ps string.
    system(ps);                         //Call terminal command

    printf("1 : to kill\n");
    printf("0 : cont\n");
    scanf("%d", &i);
    if(i == 1){
        char kill[50] = "kill ";        //Terminal command to kill parent process
        strcat(kill, Parent);           //Adds Parent PID to end of kill string.
        system(kill);                   //Call terminal command
    }
    system(ps);                         //Call terminal command to show process after kill.
}