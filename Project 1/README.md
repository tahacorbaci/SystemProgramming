# Project 1: Exit System Call
A system call which sets the value of a flag (myFlag) in the
task descriptor of a process. The myFlag flag can be 0 or 1. The system call sets the value of this flag for the process
with the given pid. Only processes having root privileges can successfully execute this system call. On error, the set_myFlag system call returns an appropriate error message. Otherwise, it returns 0.

* If the process which has myFlag=0 makes an exit system call, the default actions will be performed.
* If the process which has myFlag=1 makes an “exit” system call, all children of the exiting process will be terminated along with the process itself. Please note that this action will be performed ONLY when the “nice” value of the process with myFlag=1 is greater than 10 (i.e. priority is greater than 30). Otherwise, the “exit” system call will work normally.

You can find more information for this project [here](Project%20Description.pdf)

## Prerequisites
For this project, it is used Lubuntu 14.04.

```
sudo apt-get install manpages-dev g++ patch strace ltrace linux-headers-$ (uname -r) linux-source kernel-package fakeroot libncurses5-dev libfuse-dev
```
## Installation
### Editing the linux kernel
Move linux source archive file to desktop:
```
sudo mv /usr/src/linux-source-3.13.0/linux-source-3.13.0.tar.bz2 linux-source-3.13.0.tar.bz2
```
Extract it
```
tar -xjvf linux-source-3.13.0.tar.bz2
```
Make changes that is written in [Changed Files.txt](Changed%20Files.txt)

### Compiling the linux kernel:
```
make defconfig

make-kpkg clean

fakeroot make-kpkg --initrd --append-to-version=-custom kernel_image kernel_headers
```
If you have multi thread than you can use "-j4" for compiling with 4 thread. To use multi thread add "-j4" to end of fakeroot command.

Finally to install modified kernel:
```
sudo dpkg -i linux-image-3.13...
sudo dpkg -i linux-headers-3.13...
```

## Usage
Prototype for the system call:
```
long set_myFlag(pid_t pid, int flag);
```
## Running the tests
There are two tests. One of them is responsible for creating process and its children, and invoke system call and set nice value. The other one is responsible for showing the relation of the process and killing the parent process.

### Process Creation, System Call and Nice Value
The purpose of the test program 1 is creating three process that one of them is parent and the other two are children of this parent process. After creating process, parent process takes flag value for itself and set by using system call. If any error occurs, it shows error message. After that program asks that user wants to change nice value of the parent process. If user wants, nice value of the parent process become 11 to make nice value greater than 10. If user does not want to renice to be sure that nice value of parent process is below 10 it sets 0 the nice value. Finally, all three process waits input by using scanf function.

### Killing Process
The main goal of the test program 2 is killing process of the test program 1. To do that first it takes three pid by order of parent, child 1 and child 2. After that, it prints tree of the process and list them. If user want to kill process, it kills and list them again.

```
gcc test.c – o test.out
gcc test2.c –o test2.out

./test.out
./test2.out
```
You can see some outputs in [Report](Report.pdf)
## Contributors
* Mehmet Taha Çorbacıoğlu [Github](https://github.com/tahacorbaci)
* Kağan Özgün [Github](https://github.com/kgn95)

## License
This project is licensed under the MIT License - see the [LICENSE](../LICENSE) file for details