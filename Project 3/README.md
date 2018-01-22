# Project 3: File System
Using FUSE, it is implemented a fle system that will navigate the provided Turkish postal codes data. The fle system will analyze a CSV fle that contains this data and display a hierarchy of the neighborhoods based on the locaton names and codes.

The frst two digits of the postal code represent the city and the remaining three digits represent a neighborhood in the city. Every neighborhood is in a district of the city. The format of the CSV fle is as follows (separated by tabs):

CODE NEIGHBORHOOD CITY DISTRICT LATITUDE LONGITUDE

Example:

34398 Maslak Istanbul Sariyer 41.1083 29.0183

The path for the fle will be: NAMES → Istanbul → Sariyer → Maslak.txt.

The contents of this fle will be the data presented as key-value pairs:
```
code: 34398
neighborhood: Maslak
city: Istanbul
district: Sariyer
lattude: 41.1083
longitude: 29.0183
```
## Prerequisites
FUSE:
```
sudo apt-get install libfuse-dev
```
To parse csv:
```
libcsv.sourceforge.net
```
## Installation
```
gcc postal.c -o postal -D_FILE_OFFSET_BITS=64 -lfuse
```
## Usage
Mounting:
```
./postal <mount_dir>
```
Running in debug mode:
```
./postal -d <mount_dir>
```
Unmounting:
```
fusermount −u <mount_dir>
```
## Contributors
* Mehmet Taha Çorbacıoğlu [Github](https://github.com/tahacorbaci)
* Kağan Özgün [Github](https://github.com/kgn95)

## License
This project is licensed under the MIT License - see the [LICENSE](../LICENSE) file for details