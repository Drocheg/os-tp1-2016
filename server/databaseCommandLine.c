#include "database.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <product.h>

int printRow(void *unused, int numCols, char **colData, char **colName) {
    int i;
    for(i=0; i < numCols; i++) {
        printf("%s = %s\n", colName[i], colData[i] ? colData[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main() {
//    Product *prodArray;
//    int numProducts = db_get_products(db_connect(), &prodArray);
//    printf("%i products available:\n", numProducts);
//    for(int i = 0; i < numProducts; i++) {
//        prettyPrintProduct(prodArray[i]);
//        printf("\n");
//    }
//    return 0;
    
    
    sqlite3* conn = db_connect();
    char* errorMsg = NULL;
    char query[1024];
    while(1) {
        printf("Enter query or q to exit: ");
        int i = 0, c;
        while(i < sizeof(query)-1 && (c = getchar()) != '\n') {
            query[i++] = c;
        }
        query[i] = 0;
        if(strcmp(query, "q") == 0) {
            break;
        }
        printf("Executing \"%s\":\n", query);
        if(db_run_query(conn, query, printRow, NULL, &errorMsg) == SQLITE_OK) {
            printf("Done.\n");
        }
        else {
            printf("SQL error: %s\n", errorMsg);
            sqlite3_free(errorMsg);
        }
    }
    db_disconnect(conn);
    return 0;
}