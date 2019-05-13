#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char* readline();
char** split_string(char*);

// Complete the arrayManipulation function below.
long arrayManipulation(int n, int queries_rows, int queries_columns, int** queries) {
        int *arr = NULL;
        int i, j;
        int **ar = NULL;
//        ar = calloc(n+1, sizeof(long));
        int s1 = n,s2 =n,m1 =0,m2 =0; 
        long long max =0;
#if 0
        for( i =0 ; i < queries_rows ; i++)
            {
                arr = (queries[i]);
               if(arr[0] < s1)  {
                   s2 =s1;
                   s1 = arr[0];
               } else if(arr[0]< s2) {
                   s2 = arr[0];
               }
               if(arr[1] > m1)  {
                   m2 =m1;
                   m1 = arr[1];
               } else if(arr[1]> m2) {
                   m2 = arr[1];
               }
               
            }
            int last;
            printf("s2=%d m2 =%d \n",s2,m2);
        for( i =0 ; i < queries_rows ; i++)
            {
                arr = (queries[i]);
               if(arr[0] < s2)  {
                   j = s2;
               } else {
                   j = arr[0];

               } 

               if(arr[1] < m2)  {
                        last = arr[1];
               } else {
                   last  = m2;

               } 
               if(arr[0] == arr[1] ){
                ar[arr[0]] += arr[2];
                if (ar[arr[0]] > max) 
                max = ar[arr[0]];
               } else {

            for(; j <= last ; j++)
            {
            printf("j=%d k =%d \n",j,arr[2]);
                ar[j] += arr[2];
                if (ar[j] > max)
                max = ar[j];

            }
               if(arr[2] > max) 
                    max = arr[2];
               }
            }
            free(ar);
#else
         
        *ar = (int*)calloc(n*queries_rows, sizeof(int));
          if(ar == NULL)
          {
            perror("No memory \n");

            return -ENOMEM;
          }

        for( i =0 ; i < queries_rows ; i++)
            {
                arr = (queries[i]);
                memset(ar[i]+arr[0], arr[2],arr[1]);
            }

          free(*ar);


#endif
    return max;


}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    char** nm = split_string(readline());

    char* n_endptr;
    char* n_str = nm[0];
    int n = strtol(n_str, &n_endptr, 10);

    if (n_endptr == n_str || *n_endptr != '\0') { exit(EXIT_FAILURE); }

    char* m_endptr;
    char* m_str = nm[1];
    int m = strtol(m_str, &m_endptr, 10);

    if (m_endptr == m_str || *m_endptr != '\0') { exit(EXIT_FAILURE); }

    int** queries = malloc(m * sizeof(int*));

    for (int i = 0; i < m; i++) {
        *(queries + i) = malloc(3 * (sizeof(int)));

        char** queries_item_temp = split_string(readline());

        for (int j = 0; j < 3; j++) {
            char* queries_item_endptr;
            char* queries_item_str = *(queries_item_temp + j);
            int queries_item = strtol(queries_item_str, &queries_item_endptr, 10);

            if (queries_item_endptr == queries_item_str || *queries_item_endptr != '\0') { exit(EXIT_FAILURE); }

            *(*(queries + i) + j) = queries_item;
        }
    }

    int queries_rows = m;
    int queries_columns = 3;

    long result = arrayManipulation(n, queries_rows, queries_columns, queries);

    fprintf(fptr, "%ld\n", result);

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

char** split_string(char* str) {
    char** splits = NULL;
    char* token = strtok(str, " ");

    int spaces = 0;

    while (token) {
        splits = realloc(splits, sizeof(char*) * ++spaces);
        if (!splits) {
            return splits;
        }

        splits[spaces - 1] = token;

        token = strtok(NULL, " ");
    }

    return splits;
}
