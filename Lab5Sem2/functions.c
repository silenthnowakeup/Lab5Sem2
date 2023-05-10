//
// Created by stasc on 09.05.2023.
//

#include "functions.h"

HashItem* createItem(const char* key, const char* value) {
    HashItem* item = (HashItem*) malloc(sizeof(HashItem));
    item->key = (char*) malloc(strlen(key) + 1);
    item->value = (char*) malloc(strlen(value) + 1);
    strcpy(item->key, key);
    strcpy(item->value, value);
    item->prev = NULL;
    item->next = NULL;
    return item;
}

HashTable* createTable(int size) {
    HashTable* table = (HashTable*) malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->table = (HashItem**) calloc(size, sizeof(HashItem*));
    for (int i = 0; i < size; i++)
        table->table[i] = NULL;
    return table;
}

void freeItem(HashItem* item) {
    free(item->key);
    free(item->value);
    free(item);
}

void freeTable(HashTable* table) {
    for (int i=0; i<table->size; i++) {
        HashItem* item = table->table[i];
        if (item != NULL)
            freeItem(item);
    }

    free(table->table);
    free(table);
}

unsigned int hashFunction(const char* key, int size) {
    unsigned int hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = hash * 31 + key[i];
    }
    return hash % size;
}



void moveToFront(HashTable* table, HashItem* item) {
    if (item == table->table[hashFunction(item->key, table->size)]) {
        // элемент уже в начале списка
        return;
    }

    // обновляем указатели у соседних элементов
    if (item->prev) {
        item->prev->next = item->next;
    }
    if (item->next) {
        item->next->prev = item->prev;
    }

    // перемещаем элемент в начало списка
    item->prev = NULL;
    item->next = table->table[hashFunction(item->key, table->size)];
    if (table->table[hashFunction(item->key, table->size)]) {
        table->table[hashFunction(item->key, table->size)]->prev = item;
    }
    table->table[hashFunction(item->key, table->size)] = item;
}


HashItem* hashTableGet(HashTable* table, const char* key)
{
    unsigned long hash = hashFunction(key, table->size);
    HashItem* item = table->table[hash];

    while (item != NULL) {
        if (strcmp(item->key, key) == 0) {
            moveToFront(table, item);
            printf("%s - %s\n", item->key, item->value);
            return item;
        }
        item = item->next;
    }

    return NULL;
}

int searchByIPAddress(const HashTable* table, const char* ip_address) {
    int count = 0;

    if (!table || !ip_address) {
        return 0;
    }

    for (int i = 0; i < table->size; i++) {
        HashItem* item = table->table[i];
        while (item != NULL) {
            if (strcmp(item->value, ip_address) == 0) {
                printf("%s - %s\n", item->key, item->value);
                count++;
            }
            item = item->next;
        }
    }

    return count;
}


HashItem* removeLast(HashTable* table)
{
    HashItem* last = table->table[table->size - 1];
    while (last->next != NULL) {
        last = last->next;
    }
    if (last->prev != NULL) {
        last->prev->next = NULL;
    } else {
        table->table[table->size - 1] = NULL;
    }
    table->count--;
    return last;
}

void hashTableSet(HashTable* table, const char* key, const char* value)
{
    HashItem* item = hashTableGet(table, key);
    if (item != NULL) {
        free(item->value);
        item->value = strdup(value);
        moveToFront(table, item);
        return;
    }

    HashItem* new_item = createItem(key, value);
    new_item->next = table->table[hashFunction(key, table->size)];
    if (table->table[hashFunction(key, table->size)] != NULL) {
        table->table[hashFunction(key, table->size)]->prev = new_item;
    }
    table->table[hashFunction(key,table->size)] = new_item;

    if (table->count == table->size) {
        HashItem* last = removeLast(table);
        freeItem(last);
    } else {
        table->count++;
    }
}

void readDnsTable(HashTable* hashTable, const char* filename, const char* enterValue) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    char line[512];
    char *domainName = NULL;
    char *ipAddress = NULL;
    char *cname = NULL;
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, " ");
        while (token != NULL) {
            if (strncmp(token, "A:", 2) == 0) {
                free(ipAddress);
                ipAddress = strdup(token + 2);
                ipAddress[strcspn(ipAddress,"\n")] = '\0';
            } else if (strncmp(token, "CNAME:", 6) == 0) {
                free(cname);
                cname = strdup(token + 6);
                cname[strcspn(cname, "\n")] = '\0';
            } else if (strncmp(token, "IN:", 3) == 0) {
                free(domainName);
                domainName = strdup(token + 3);
            }

            token = strtok(NULL, " ");
        }

        if (cname != NULL && domainName != NULL) {
            HashItem *item = hashTableGet(hashTable, cname);
            if (item != NULL) {
                free(ipAddress);
                ipAddress = strdup(item->value);
            }
        }

        if ((domainName != NULL && ipAddress != NULL) && strcmp(domainName, enterValue) == 0) {
            hashTableSet(hashTable, domainName, ipAddress);
            free(cname);
            cname = NULL;
            free(domainName);
            domainName = NULL;
            free(ipAddress);
            ipAddress = NULL;
        }
    }

    fclose(file);

    free(cname);
    free(domainName);
    free(ipAddress);
}



void findIP(HashTable* hashTable, const char* filename, const char* enterValue) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    char line[512];
    char *saveptr1 = NULL;
    char *saveptr2 = NULL;
    char *domainName = NULL;
    char *ipAddress = NULL;
    char *cname = NULL;

    while (fgets(line, sizeof(line), file)) {
        const char *token = strtok_r(line, " ", &saveptr1);
        domainName = NULL;
        ipAddress = NULL;
        cname = NULL;

        while (token != NULL) {
            if (strncmp(token, "A:", 2) == 0) {
                ipAddress = strdup(token + 2);
                ipAddress[strcspn(ipAddress, "\n")] = '\0';
            } else if (strncmp(token, "CNAME:", 6) == 0) {
                cname = strdup(token + 6);
                cname[strcspn(cname, "\n")] = '\0';
            } else if (strncmp(token, "IN:", 3) == 0) {
                domainName = strdup(token + 3);
            }

            token = strtok_r(NULL, " ", &saveptr1);
        }

        if ((domainName != NULL && ipAddress != NULL) && strcmp(enterValue, ipAddress) == 0) {
            const char *tempDomainName = strdup(domainName); // create a copy to avoid modifying the original
            hashTableSet(hashTable, domainName, ipAddress);
            fseek(file, 0, SEEK_SET);

            while (fgets(line, sizeof(line), file)) {
                const char *tmptoken = strtok_r(line, " ", &saveptr2);
                char *tmpdomainName = NULL;
                char *tmpipAddress = NULL;
                char *tmpcname = NULL;

                while (tmptoken != NULL) {
                    if (strncmp(tmptoken, "A:", 2) == 0) {
                        tmpipAddress = strdup(tmptoken + 2);
                        tmpipAddress[strcspn(tmpipAddress, "\n")] = '\0';
                    } else if (strncmp(tmptoken, "CNAME:", 6) == 0) {
                        tmpcname = strdup(tmptoken + 6);
                        tmpcname[strcspn(tmpcname, "\n")] = '\0';
                    } else if (strncmp(tmptoken, "IN:", 3) == 0) {
                        tmpdomainName = strdup(tmptoken + 3);
                    }

                    if (tmpcname != NULL && strcmp(tmpcname, tempDomainName) == 0) {
                        hashTableSet(hashTable, tmpdomainName, ipAddress);
                    }

                    tmptoken = strtok_r(NULL, " ", &saveptr2);
                }

                free(tmpipAddress);
                free(tmpcname);
                free(tmpdomainName);
            }

            free((char*)tempDomainName);
        }

        free(ipAddress);
        free(cname);
        free(domainName);
    }

    fclose(file);
}








void hashTablePrint(const HashTable* table) {
    if (table == NULL) {
        printf("Hash table is NULL\n");
        return;
    }

    for (int i = 0; i < table->size; i++) {
        HashItem* item = table->table[i];
        while (item != NULL) {
            printf("%s - %s\n", item->key, item->value);
            item = item->next;
        }
    }
}

bool isValidIp(const char* ip) {
    int num = 0;
    int dots = 0;

    while (*ip) {
        if (*ip == '.') {
            dots++;
            if (num < 0 || num > 255 || dots > 3) {
                return false;
            }
            num = 0;
        } else if (*ip >= '0' && *ip <= '9') {
            num = num * 10 + (*ip - '0');
            if (num < 0 || num > 255) {
                return false;
            }
        } else {
            return false;
        }
        ip++;
    }

    if (dots != 3 || num < 0 || num > 255) {
        return false;
    }

    return true;
}


bool checkDuplicate(const char* filename, const char* domain) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return false;
    }

    char line[256];
    char *saveptr1;
    while (fgets(line, sizeof(line), file)) {
        const char *token = strtok_r(line, " ", &saveptr1);
        const char *domainName = NULL;
        while (token != NULL) {
            if (strncmp(token, "IN:", 3) == 0) {
                domainName = strdup(token + 3);
            }
            if (domainName != NULL && strcmp(domainName, domain) == 0)
                return true;

            token = strtok_r(NULL, " ", &saveptr1);
        }
    }
    fclose(file);
    return false;
}







void saveFile(const char* filename, const char* domain, const char* ip,unsigned int k)
{
    FILE* file = fopen(filename,"a");

    if (!isValidIp(ip)) {
        printf("Invalid IP address: %s\n", ip);
        return;
    }
    if (checkDuplicate(filename, domain)) {
        printf("Record already exists in the file.\n");
        return;
    }

    if (k==1)
    {
        fprintf (file,"IN:%s A:%s\n",domain, ip);
    }
    else if (k==2)
    {
        fprintf(file,"IN:%s CNAME:%s\n",domain, ip);
    }
    fclose(file);
}


char* inputStr() {
    char* str = NULL;
    int pos = 0;
    int len = 0;
    int c = getchar();

    while (c != '\n') {
        if (pos == len) {
            len += 10;
            str = (char*) realloc(str, len * sizeof(char));
        }

        str[pos] = (char)c;
        pos++;
        c = getchar();
    }

    if (pos == len) {
        len++;
        str = (char*) realloc(str, len * sizeof(char));
    }

    str[pos] = '\0';
    return str;
}

int menu()
{
    int value;
    printf("1. Search IP by domain\n");
    printf("2. Save Info in File\n");
    printf("3. Search domain by IP\n");
    printf("4. Output all hash-table\n");
    printf("5. Exit\n");

    while (scanf("%d", &value) != 1 || getchar() != '\n' || value <= 0 || value>4)
    {
        printf("Wrong input!\n");
        rewind(stdin);

    }
    return value;
}
