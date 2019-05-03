
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();
char** split_string(char*);
void minimumBribes(int q_count, int* q) {
  int i,j;
  int bribes = 0;
  int diff = 0;
  int swap ;
#define PREV      i-1 
#define CURNT     i   
#define OFFSET    i+1            

void minimumBribes(int q_count, int* q) {

    int i,j;
    int b = 0;
    int diff;
    int temp;

    for( i = q_count-1 ; i > 0 ; i--){
        //printf("i=%d q[%d]=%d \n",i,i,q[i]);
                if(q[i] == i+1)
                {
        //            printf("i=%d continue \n",i);
                    continue;
                } else if(q[i-1] == i+1 ) {
       // printf("B:i=%d q[%d]=%d q[%d]=%d\n",i,i,q[i],i-1,q[i-1]);
                  temp = q[i];
                  q[i] = q[i-1];
                  q[i-1] = temp;
                  b++;
      //              printf("One Bribe :i=%d b=%d\n",i,b);
       // printf("A:i=%d q[%d]=%d q[%d]=%d\n",i,i,q[i],i-1,q[i-1]);
              } else if (q[i-2]== i+1){
       // printf("B:i=%d q[%d]=%d q[%d]=%d\n",i,i,q[i],i-2,q[i-2]);
                  if(q[i-1] < q[i]) {
                  temp = q[i-2];
                  q[i-2] = q[i-1];
                  q[i-1] = temp;
                  temp  = q[i];
                  q[i] = q[i-1];
                  q[i-1] = temp;                 
                  b += 2;
                  } else{
                      temp = q[i];
                      q[i] = q[i-2];
                      q[i-2] = temp;
                  b += 3;
                  }
                  //  printf("two Bribe :i=%d b=%d\n",i,b);
     //   printf("A:i=%d q[%d]=%d q[%d]=%d\n",i,i,q[i],i-2,q[i-2]);
              }else {
                  printf("Too chaotic\n");
                  return;
              }
    }
    printf("%d\n",b);
    return;
}
  

}

// Complete the minimumBribes function below.
void minimumBribes(int q_count, int* q) {

    int i = 0;
    int min_bribes =0;
    int too_chao = 0;
    int diff =0;
    int temp;

    for(i = q_count -1 ; i >= 0; i--)
    {
            if((q[i] != i-1))
            {
//                min_bribes++;
        diff = abs(q[i] - (i+1));
                        printf("q[%d]=%d i=%d diff =%d \n",i,q[i],i,diff);
            if(diff > 2){
                too_chao = 1;
                goto chao;

            }
            }else {
                        printf("q[%d]=%d i=%d \n",i,q[i],i);

            }

    }
    int j;
    for(i = 0 ; i < q_count; i++)
    {
           for (j = 0; j < q_count; j++) {
        if(q[j] > q[j+1]) {
            temp = q [j];
            q[j] = q [j+1];
            q [j+1] = temp;
            min_bribes++;
        }
           }

    }









chao:
    if(too_chao)
    printf("Too chaotic\n");
    else 
    printf("%d\n",min_bribes);





}

int main()
{
    char* t_endptr;
    char* t_str = readline();
    int t = strtol(t_str, &t_endptr, 10);

    if (t_endptr == t_str || *t_endptr != '\0') { exit(EXIT_FAILURE); }

    for (int t_itr = 0; t_itr < t; t_itr++) {
        char* n_endptr;
        char* n_str = readline();
        int n = strtol(n_str, &n_endptr, 10);

        if (n_endptr == n_str || *n_endptr != '\0') { exit(EXIT_FAILURE); }

        char** q_temp = split_string(readline());

        int* q = malloc(n * sizeof(int));

        for (int i = 0; i < n; i++) {
            char* q_item_endptr;
            char* q_item_str = *(q_temp + i);
            int q_item = strtol(q_item_str, &q_item_endptr, 10);

            if (q_item_endptr == q_item_str || *q_item_endptr != '\0') { exit(EXIT_FAILURE); }

            *(q + i) = q_item;
        }

        int q_count = n;

        minimumBribes(q_count, q);
    }

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
