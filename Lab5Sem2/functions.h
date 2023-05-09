//
// Created by stasc on 09.05.2023.
//

#ifndef LAB5SEM2_FUNCTIONS_H
#define LAB5SEM2_FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct HashItem
{
    char* key;
    char* value;
    struct HashItem* prev;
    struct HashItem* next;
}HashItem;

typedef struct HashTable{
    int size;
    int count;
    HashItem** table;
    HashItem* list_head;
    HashItem* list_tail;
}HashTable;

HashItem* createItem(const char* key,const char* value);
HashTable* createTable(int size);
void freeItem(HashItem* item);
void freeTable(HashTable* table);
bool isValidIp(const char *ipAddress);
bool checkDuplicate(const char* filename, const char* domain);
unsigned int hashFunction(const char* str, int size);
void moveToFront(HashTable* table, HashItem* item);
HashItem* hashTableGet(HashTable* table, const char* key);
HashItem* removeLast(HashTable* table);
void hashTableSet(HashTable* table, const char* key, const char* value);
void readDnsTable(HashTable* hashTable, const char* filename, const char* enterValue);
void findIP(HashTable* hashTable, const char* filename, const char* enterValue);
void hashTablePrint(HashTable* table);
void saveFile(const char* filename, const char* domain, const char* ip,unsigned int k);
char* inputStr();
int menu();
int searchByIPAddress(HashTable* table,const char* ip_address);
#endif //LAB5SEM2_FUNCTIONS_H
