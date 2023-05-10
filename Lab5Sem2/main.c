#include "functions.h"


int main() {
    const char* filename;
    const char* enterValue;
    unsigned int k;
    HashTable* table = createTable(3);
    const char* domain;
    const char* ip;
    int count;

while(1)
{
    switch(menu())
    {
        case 1:
            printf("\nEnter Value::");
            enterValue = inputStr();
            const HashItem* item = hashTableGet(table, enterValue);
            if (item == NULL) {
                printf("\nThis value not found in hash-table");
                printf("\nEnter filename::");
                filename = inputStr();
                readDnsTable(table, filename, enterValue);
            }
            break;
        case 2:
            printf("Enter filename::");
            filename = inputStr();
            printf("\nEnter domain::");
            domain = inputStr();
            printf("\nEnter what you want to add:: (1. Domain with Ip,  Domain with CNAME) :: ");
            scanf("%d",&k);
            int temp = getchar();
            printf("\nEnter your Value::");
            enterValue = inputStr();
            saveFile(filename,domain,enterValue,k);
            break;

        case 3:
            printf("\nEnter IP-address: ");
            ip = inputStr();
            count = searchByIPAddress(table,ip);
            if (count == 0) {
                printf("\nThis value not found in hash-table");
                printf("\nEnter filename: ");
                filename = inputStr();
                findIP(table, filename, ip);
            }
            break;
        case 4:
            hashTablePrint(table);
            break;
        default:
            freeTable(table);
            return 0;
    }
}
}
