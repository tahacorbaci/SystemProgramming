/*
Mehmet Taha Çorbacıoğlu - 150130017
Kağan Özgün - 150130055
*/
#include <stdio.h>
#include <string.h>

int structSize = 0;
struct ZipData{
	char code[10];
	char neighborhood[20];
	char city[20];
	char district[20];
	char latitude[20];
	char longitude[20];
};
struct ZipData head[36309];
void printList(struct ZipData *head){
	int i =0;
	while ( i< structSize){
		i++;
		printf("%s %s\n",head[i].code, head[i].city);
	}
}
int cityNum = 0;
struct city{
	char name[20];
	struct city *next;
};
struct city *cityHead = NULL;
struct city *cityEnd = NULL;
void insertCity(char *city){
	struct city *newNode = (struct city*)malloc(sizeof(struct city));
	strcpy(newNode->name, city);
	newNode->next = NULL;
	if(cityHead == NULL){
		cityHead = newNode;
		cityEnd = newNode;
	}else{
		cityEnd->next = newNode;
		cityEnd = newNode;
	}
	cityNum++;
}

int nodeNum = 0;
struct node{
	char name[20];
	struct node *next;
};
struct node *nodeHead = NULL;
struct node *nodeEnd = NULL;
void insertnode(char *node){
	struct node *newNode = (struct node*)malloc(sizeof(struct node));
	strcpy(newNode->name, node);
	newNode->next = NULL;
	if(nodeHead == NULL){
		nodeHead = newNode;
		nodeEnd = newNode;
	}else{
		nodeEnd->next = newNode;
		nodeEnd = newNode;
	}
	nodeNum++;
}
void cleannode(){
	struct node *newNode;
	while(nodeHead != NULL){
		newNode = nodeHead;
		nodeHead = nodeHead->next;
		free(newNode);
	}
	nodeNum = 0;
}

struct plateCode{
	char name[3];
	struct plateCode *next;
};
struct plateCode plist[36309];
struct plateCode *plateHead = NULL;
struct plateCode *plateEnd = NULL;
int plateNum = 0;
void insertPlateCode(char *p){
	struct plateCode *newNode = (struct plateCode*)malloc(sizeof(struct plateCode));
	strcpy(newNode->name, p);
	newNode->next = NULL;
	if(plateHead == NULL){
		plateHead = newNode;
		plateEnd = newNode;
	}else{
		plateEnd->next = newNode;
		plateEnd = newNode;
	}
	plateNum++;
}

struct printN{
	char neighborhood[50];
	char messages[200];
	struct printN *next;
};
struct printN *pHead = NULL;
struct printN *pEnd = NULL;
int printNum = 0;
void insertPrintN(char *n, char *m){
	struct printN *newNode = (struct printN*)malloc(sizeof(struct printN));
	strcpy(newNode->neighborhood, n);
	strcpy(newNode->messages, m);
	newNode->next = NULL;
	if(pHead == NULL){
		pHead = newNode;
		pEnd = newNode;
	}else{
		pEnd->next = newNode;
		pEnd = newNode;
	}
	printNum++;
}
