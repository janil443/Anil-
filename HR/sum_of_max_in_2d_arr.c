int hourglassSum(int arr_rows, int arr_columns, int** arr) {
    int max =0, c,n,r,sum;
    n = (arr_rows - 2) * (arr_columns -2);

    for (c = 0; c < (arr_columns -2) ; c++ )
    {

        for (r = 0; r < (arr_rows - 2) ; r++){

            sum = (arr[r][c]+arr[r][c+1]+arr[r][c+2]+ arr[r+1][c+1]+arr[r+2][c]+arr[r+2][c+1]+arr[r+2][c+2]);

            if (sum > max)
                max = sum;

        }
    }
    return max;
}
