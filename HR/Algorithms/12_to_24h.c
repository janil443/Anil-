
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();

/*
 * Complete the timeConversion function below.
 */

/*
 * Please either make the string static or allocate on the heap. For example,
 * static char str[] = "hello world";
 * return str;
 *
 * OR
 *
 * char* str = "hello world";
 * return str;
 *
 */
char* timeConversion(char* s) {
    /*
     * Write your code here.
     */
     static char time[9];
     strncpy(time, s, 8);
     if((s[8] == 'A') && s[0] == '1' && s[1] == '2'){
       printf(" 1st condtion");
       time[0] = '0';
       time[1] = '0';
     } else if((s[8] == 'P') && !((s[0] == '1' ) && (s[1] == '2'))){
       printf(" 2st condtion");

        if((s[0] == '0') && s[1] < 56)
        {
       printf(" 2st(1) condtion");
          time[0] = '1';
          time[1] = s[1]+2;
        }else if(s[0] == '1' && s[1] < 50 ) {  
          time[0] = '2';
          time[1] = s[1] + 2;

        } else{                                         
       printf(" 2st(2) condtion");
          time[0] = '2';
          time[1] = s[1] - 8;
        } 

     }else{
       printf(" 3rd condtion");
     }

return time;
}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    char* s = readline();

    char* result = timeConversion(s);

    fprintf(fptr, "%s\n", result);

    fclose(fptr);

    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;
    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) { break; }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') { break; }

        size_t new_length = alloc_length << 1;
        data = realloc(data, new_length);

        if (!data) { break; }

        alloc_length = new_length;
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';
    }

    data = realloc(data, data_length);

    return data;
}
