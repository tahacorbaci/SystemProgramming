# Project 2: Device Driver

A device driver that will act as a simple message box between the users on a system. When a user reads from the device, he/she will only see the messages addressed to him/her.

For example, if the user "alice" wants to send a message to the user "bob" to say "Hello", she will write to the device the text "@bob Hello". Later, when user "bob" reads from the device he will see the message as "alice: Hello". Although the message box is global for all users, user "bob" will not see any messages sent to other users.

## Prerequisites
For this project, it is used Lubuntu 14.04.

```
sudo apt-get install g++ libfuse-dev
```

## Installation
To compile the device driver:
```
make
```
Loading:
```
insmod ./proje.ko
```
Creating the device nodes (assuming major number is 250):
```
mknod /dev/proje0 c 250 0
```

## Usage
Writing to the device:
```
echo @user1 testing > /dev/proje0
```
Reading from the device:
```
cat /dev/proje0
```

## Contributors
* Mehmet Taha Çorbacıoğlu [Github](https://github.com/tahacorbaci)
* Kağan Özgün [Github](https://github.com/kgn95)

## License
This project is licensed under the MIT License - see the [LICENSE](../LICENSE) file for details