#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int main(int argc, char* argv[])
{
    if (argc != 10)
    {
        printf("ERROR");
        return EXIT_FAILURE;
    }

    // Get the input files
    FILE* inputMatrix1 = fopen(argv[1], "r");
    FILE* inputMatrix2 = fopen(argv[4], "r");

    char* p1;
    char* p2;

    // Get matrix 1's dims
    int n_row1 = strtol(argv[2], &p1, 10);
    int n_col1 = strtol(argv[3], &p2, 10);

    // Get matrix 2's dims
    int n_row2 = strtol(argv[5], &p1, 10);
    int n_col2 = strtol(argv[6], &p2, 10);

    // Get num threads
    int thread_count = strtol(argv[9], NULL, 10);

    // Get output files
    FILE* outputFile = fopen(argv[7], "w");
    FILE* outputTime = fopen(argv[8], "w");


    // TODO: malloc the two input matrices
    // Please use long int as the variable type
    long *matrix1 = malloc(n_row1 * n_col1 * sizeof(long));
    long *matrix2 = malloc(n_row2 * n_col2 * sizeof(long));

     //parsing first file and fill in first matrix
    for(int i = 0; i < n_row1; ++i){
        for(int j = 0; j < n_col1; ++j){
            fscanf(inputMatrix1, "%ld,", &matrix1[i * n_col1 + j]);
        }
    }

    //parsing second file and fill in second matrix
    for(int i = 0; i < n_row2; ++i){
        for(int j = 0; j < n_col2; ++j){
            fscanf(inputMatrix2, "%ld,", &matrix2[i * n_col2 + j]);
        }
    }

    double start = omp_get_wtime();

    //initializing starting variables
    long largest = 0;
    long secondLargest = 0;

    #pragma omp parallel num_threads(thread_count)
    { 
        // local versions for each thread
        long localLargest = 0;
        long localSecond = 0;

        #pragma omp for
        for(int j = 0; j < n_col2; ++j) { 
            for(int i = 0; i < n_row1; ++i) {   
                long x = 0; //just need to store in variable, not matrix

                //dot product
                for(int k = 0; k < n_col1; ++k){
                    x += matrix1[i * n_col1 + k] * matrix2[k * n_col2 + j];
                }

                // handling both scenarios of x
                if (x > localLargest){
                    localSecond = localLargest;
                    localLargest = x;
                }
                else if(x > localSecond)
                    localSecond = x;
            }
        }

        // updating shared variables largest and second largest
        #pragma omp critical
        {
            if (localLargest > largest) {
                //current largest gets demoted to second
                if (largest > localSecond) {
                    secondLargest = largest;
                } 
                else {
                    secondLargest = localSecond;
                }

                largest = localLargest;
            } 
            else if (localLargest > secondLargest) {
                secondLargest = localLargest;
            } 
            else if (localSecond > secondLargest) {
                secondLargest = localSecond;
            }
        }
    }



    double end = omp_get_wtime();
    double time_passed = end - start;

    //save results
    fprintf(outputFile, "%ld\n", secondLargest);
    fprintf(outputTime, "%f", time_passed);

    // Cleanup
    fclose(inputMatrix1);
    fclose(inputMatrix2);
    fclose(outputFile);
    fclose(outputTime);

    //free buffers
    free(matrix1);
    free(matrix2);

    return 0;
}
