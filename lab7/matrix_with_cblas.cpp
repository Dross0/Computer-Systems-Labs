#include <cblas.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#define SIZE 1024
#define M 10

using namespace std;

float maxFirst(float *matrix) {
    float sums[SIZE];
    for (int i = 0; i < SIZE; ++i) {
	   sums[i] = cblas_sasum(SIZE, matrix + SIZE * i, 1);
    }
    return sums[cblas_isamax(SIZE, sums, 1)];
}

float maxInfinity(float *matrix) {
    float sums[SIZE];
    for (int i = 0; i < SIZE; ++i) {
	   sums[i] = cblas_sasum(SIZE, matrix + i, SIZE);
    }
    return sums[cblas_isamax(SIZE, sums, 1)];
}


void identity(float* result){
    for (int i = 0; i < SIZE; ++i){
	   result[i * (SIZE + 1)] = 1;
    }
}

int main() {
    float *matrixA = (float *)calloc(SIZE * SIZE, sizeof(float));
    float *matrixR = (float *)calloc(SIZE * SIZE, sizeof(float));
    float *matrixI = (float *)calloc(SIZE * SIZE, sizeof(float));
    float *matrixM = (float *)calloc(SIZE * SIZE, sizeof(float));
    float *matrixT = (float *)calloc(SIZE * SIZE, sizeof(float));
    if (matrixA == NULL || matrixR == NULL || matrixI == NULL || matrixM == NULL || matrixT == NULL){
        printf("MALLOC RETURNED NULL\n");
        return 1;
    }
    float max_1, max_inf;
    for (int i = 0; i < SIZE; ++i) {
	   for (int j = 0; j < SIZE; ++j) {
	       matrixA[i * SIZE + j] = (float)rand() / RAND_MAX * 10;
	   }
    }

    clock_t start, end, start1, end1;
    start = clock();

    max_1 = maxFirst(matrixA);
    max_inf = maxInfinity(matrixA);

    cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, SIZE, SIZE, SIZE, 1.0 / (max_1 * max_inf), matrixA, SIZE, matrixA, SIZE, 0, matrixR, SIZE); // умножение матриц

    identity(matrixI);
    cblas_saxpy(SIZE * SIZE, (float)-1, matrixI, 1, matrixR, 1);
    memcpy(matrixM, matrixR, SIZE * SIZE * sizeof(float));
    start1 = clock();
    for (int i = 0; i < M; ++i) {
	   cblas_saxpy(SIZE * SIZE, (float)1, matrixM, 1, matrixI, 1);
	   cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, SIZE, SIZE, SIZE, (float)1, matrixM, SIZE, matrixR, SIZE, 0, matrixT, SIZE);
	   matrixM = matrixT;
    }
    end1 = clock() - start1;
    cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, SIZE, SIZE, SIZE, (float)1, matrixA, SIZE, matrixI, SIZE, 0, matrixT, SIZE);

    end = clock() - start;
    double circle = (double) end1 / CLOCKS_PER_SEC;
    double all = (double) end / CLOCKS_PER_SEC;
    cout.precision(6);
    cout << "Time of circle (CBLAS): " << circle << endl;
    cout << "All time (CBLAS): " << all << endl << endl;
    free(matrixA);
    free(matrixR);
    free(matrixI);
    free(matrixM);

    return 0;
}