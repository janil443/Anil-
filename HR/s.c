int s_len =0;
    int i = 0;
    long j=0 ;
    int k = 0;
    s_len = strlen(s);

    for(i = 0; i < s_len ;i++)
    {
            if(s[i] == 'a') {
            j++;
       }
    }
    if( n % s_len == 0 && s_len < n){
        j = j * (n/s_len);
       } else if (s_len < n){

           s_len = n % s_len;
           for( i =0 ; i < s_len ; i++) {
               if(s[i] == 'a')
               k++;
           }
           j = (j*(n/strlen(s))) + k; 


       } if (s_len > n) {
           j = 0;
           s_len = (s_len -  n);
           for( i = 0; i < s_len; i++){
               if(s[i] == 'a') {
                   j++;
               }
           }
       }
        

return j;
