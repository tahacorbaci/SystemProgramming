/*
Mehmet Taha Çorbacıoğlu - 150130017
Kağan Özgün - 150130055
To compile: gcc postal.c -o postal -D_FILE_OFFSET_BITS=64 -lfuse -Ldir "/usr/local/lib/libcsv.a"
*/
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include "csv.h"
#include "linkedlist.h"

char *rw_path;
static char *message;
static char *txtName;
static char *cityName;
static const char *postalCodes = "/CODES";
static const char *postalNames = "/NAMES";

void cb1 (void *s, size_t len, void *data) {
    char *line = (char *) malloc(strlen((char*)s)*sizeof(char));
    strcpy(line, (char*)s);
    char *words;
    words = strtok (line,"\t");
	int i = 0;
    while (words != NULL){
		switch(i){
			case 0:
				strcpy(head[structSize].code, words);
				char temp[3];
				temp[0] = words[0];
				temp[1] = words[1];
				temp[2] = '\0';
				plist[structSize].name[0] = '\0';
				strcpy(plist[structSize].name,temp);
				break;
			case 1:
				strcpy(head[structSize].neighborhood, words);
				break;
			case 2:
				strcpy(head[structSize].city, words);
				break;
			case 3:
				strcpy(head[structSize].district, words);
				break;
			case 4:
				strcpy(head[structSize].latitude, words);
				break;
			case 5:
				strcpy(head[structSize].longitude, words);
				break;
		}
		i++;
        words = strtok (NULL, "\t");
    }
	structSize++;
	free(line);
	free(words);
}
void cb2 (int c, void *data) {
}

static int is_space(unsigned char c) {
  if (c == CSV_SPACE || c == CSV_TAB) return 1;
  return 0;
}

static int is_term(unsigned char c) {
  if (c == CSV_CR || c == CSV_LF) return 1;
  return 0;
}
void cityDIR(){
	int i;
	int found;
	struct city *traverse = cityHead;
	for(i = 0; i < structSize;i++){
		while(traverse != NULL){
			if(strcmp(head[i].city,traverse->name) == 0){
				found = 1;
				break;
			}
			else{
				found = 0;
				traverse = traverse->next;
			}	
		}
		if(found == 0){
			insertCity(head[i].city);
			traverse = cityHead;
		}
	}
}

void districtDIR(char * cityDirName){
	int i;
	int found = 0;
	struct node *traverse = nodeHead;
	for(i = 0; i < structSize;i++){
		while(traverse != NULL){
			if(strcmp(head[i].district,traverse->name) == 0){
				found = 1;
				break;
			}
			else{
				found = 0;
				traverse = traverse->next;
			}	
		}
		if((found == 0)&& (strcmp(head[i].city, cityDirName) == 0)){
			insertnode(head[i].district);
			traverse = nodeHead;
		}
	}
}
void plateDIR(){
	int i, j;
	int found;
	struct plateCode *traverse2 = plateHead;
	for(i = 0; i < structSize;i++){
		while(traverse2 != NULL){
			if(strcmp(plist[i].name,traverse2->name) == 0){
				found = 1;
				break;
			}
			else{
				found = 0;
				traverse2 = traverse2->next;
			}	
		}
		if(found == 0){
			insertPlateCode(plist[i].name);
			traverse2 = plateHead;
		}
	}
}
void readCSV(){
	FILE *fp = fopen("/home/taha/Desktop/v6/postal-codes.csv", "rb");
    if (!fp) {
      fprintf(stderr, "Failed to open \n");
    }    
    struct csv_parser p;
    char buf[1024];
    size_t bytes_read;

    if (csv_init(&p, 0) != 0) {
        fprintf(stderr, "Failed to initialize csv parser\n");
    }
    csv_set_space_func(&p, is_space);
    csv_set_term_func(&p, is_term);

    while ((bytes_read=fread(buf, 1, 1024, fp)) > 0) {
      if (csv_parse(&p, buf, bytes_read, cb1, cb2, NULL) != bytes_read) {
        fprintf(stderr, "Error while parsing file: %s\n", csv_strerror(csv_error(&p)));
      }
    }

    fclose(fp);
    csv_free(&p);
}


static int postal_getattr(const char *path, struct stat *stbuf){
    int res = 0;
	int i, count;
	for (i=0, count=0; path[i]; i++) // Count "/" delim
		count += (path[i] == '/');
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path,"/") == 0) 
    {		
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 5;
    }
    else if(strcmp(path,postalCodes) == 0)
    {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 4;		
	}
	else if(strcmp(path,postalNames) == 0)
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 4;
	}
	else if(count == 2) // Directory of Number or City
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 3;
	}
	else if(count == 3) // Directory of District or File of code directory
	{
		if (strstr(path, "/CODES") == NULL){
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
		else{
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = 150;
		}			    
	}
    else if(count == 4) // File of District directory
	{
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = 150;
	}
    else
        res = -ENOENT;

    return res;
}
static int postal_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
    (void) offset;
    (void) fi;
	int i, count;
	for (i=0, count=0; path[i]; i++){
		count += (path[i] == '/'); // Count "/" delim
	}
		
	
    if(strcmp(path, "/") == 0)
    {
		filler(buf, postalCodes + 1, NULL, 0);		
		filler(buf, postalNames + 1, NULL, 0);
	}
	else if(strcmp(path, "/CODES") == 0)
    {
		plateDIR();
		struct plateCode *traverse = plateHead;
		for(i = 0; i < plateNum; i++){
			filler(buf, traverse->name, NULL, 0);
			traverse = traverse->next;
		}
	}
	else if(count == 2 && path[1] == 'C')
    {
		char *fileName;
		for(i = 0; i < structSize; i++){
			if(strstr(path, plist[i].name) != NULL && head[i].code[0] != 'a'){
				fileName = (char*)malloc((strlen(head[i].code) + strlen(".txt") + 1)*sizeof(char));
				strcpy(fileName, head[i].code);
				strcat(fileName, ".txt");
				if((i > 0)&&(strcmp(head[i].code,head[i-1].code) ==0)){
					continue;
				}else{
					filler(buf, fileName, NULL, 0);
				}
				free(fileName);
			}
		}
	}
	else if(strcmp(path, "/NAMES") == 0)
    {
		cityDIR();
		struct city *traverse = cityHead;
		for(i = 0; i < cityNum; i++){
			filler(buf, traverse->name, NULL, 0);
			traverse = traverse->next;
		}
	}
	else if(count == 2 && path[1] == 'N')
    {
		char *cityDirName = (char *) malloc((strlen(path) - 6)*sizeof(char));
		strncpy(cityDirName, path+7, strlen(path) - 6);
		districtDIR(cityDirName);
		struct node *traverse = nodeHead;
		for(i = 0; i < nodeNum; i++){
			filler(buf, traverse->name, NULL, 0);
			traverse = traverse->next;
		}
		cleannode();
		free(cityDirName);
	}
	else if(count == 3 && path[1] == 'N')
    {
		char *fileName;
		for(i = 0; i < structSize; i++){
			if(strstr(path, head[i].city) != NULL && strstr(path, head[i].district) != NULL && head[i].neighborhood[0] != '0'){
				fileName = (char*)malloc((strlen(head[i].neighborhood) + strlen(".txt") + 1)*sizeof(char));
				strcpy(fileName, head[i].neighborhood);
				strcat(fileName, ".txt");
				filler(buf, fileName, NULL, 0);
				free(fileName);
			}
		}
	}

	else
		return -ENOENT;
	return 0;
}
static int postal_open(const char *path, struct fuse_file_info *fi){

    return 0;
}
static int postal_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){ 
    size_t len;
    (void) fi;
	int i = 0;
	int location = 0;
	int count = 0;
	message = (char*)malloc(150*sizeof(char));
	txtName = (char*)malloc(50*sizeof(char));
	for (i=0, count=0; path[i]; i++){
		count += (path[i] == '/'); // Count "/" delim
		if(count == 3){
			location = i;
			break;
		}
	}
	cityName = (char*)malloc((location - 6)*sizeof(char));
	cityName[0] = '\0';
	strncpy(cityName, path +7, location - 7);
	cityName[location - 7] = '\0';
	for(i = 0; i < structSize; i++){
		txtName[0] = '\0';
		strcat(txtName, head[i].neighborhood);
		strcat(txtName, ".txt");
		
		if(((strstr(path, txtName) != NULL) && (strcmp(head[i].city, cityName) == 0)) || ((strstr(path,"CODES") != NULL) && (strstr(path,head[i].code) != NULL))){
			message[0] = '\0';
			strcat(message, "code: ");
			strcat(message, head[i].code);
			strcat(message, "\n");
			strcat(message, "neighborhood: ");
			strcat(message, head[i].neighborhood);
			strcat(message, "\n");
			strcat(message, "city: ");
			strcat(message, head[i].city);
			strcat(message, "\n");
			strcat(message, "district: ");
			strcat(message, head[i].district);
			strcat(message, "\n");
			strcat(message, "latitude: ");
			strcat(message, head[i].latitude);
			strcat(message, "\n");
			strcat(message, "longitude: ");
			strcat(message, head[i].longitude);
			strcat(message, "\n");
			message[strlen(message)] = '\0';
			len = strlen(message);
			printf("\n\n\n path: %s\n\n\n", path);
			printf("\n\n\n message: %s\n\n\n", message);
			printf("\n\n\n i: %d\n\n\n",i);
			if (offset < len) {
				if (offset + size > len)
					size = len - offset;
				memcpy(buf, message + offset, size);
			} else
				size = 0;
			break;
		}
	}
	free(txtName);
	free(cityName);
	free(message);
    return size;
}
static int postal_unlink(const char *path)
{
    (void)path;
	int i = 0;
	int location = 0;
	int count = 0;
	txtName = (char*)malloc(50*sizeof(char));
	for (i=0, count=0; path[i]; i++){
		count += (path[i] == '/'); // Count "/" delim
		if(count == 3){
			location = i;
			break;
		}
	}
	cityName = (char*)malloc((location - 6)*sizeof(char));
	cityName[0] = '\0';
	strncpy(cityName, path +7, location - 7);
	cityName[location - 7] = '\0';
	for(i = 0; i < structSize; i++){
		txtName[0] = '\0';
		strcat(txtName, head[i].neighborhood);
		strcat(txtName, ".txt");
		if(((strstr(path, txtName) != NULL) && (strcmp(head[i].city, cityName) == 0)) || ((strstr(path,"CODES") != NULL) && (strstr(path,head[i].code) != NULL))){
			head[i].code[0] = 'a';
			head[i].code[1] = '\0';
			head[i].neighborhood[0] = '0';
			head[i].neighborhood[1] = '\0';
		}
	}
	free(txtName);
	free(cityName);
	return 0;
}
static struct fuse_operations postal_oper = {
    .getattr	= postal_getattr,
    .readdir	= postal_readdir,
	.unlink     = postal_unlink,
    .open		= postal_open,
    .read		= postal_read,
};
int main(int argc, char *argv[]){
	readCSV();
    return fuse_main(argc, argv, &postal_oper, NULL);
}
