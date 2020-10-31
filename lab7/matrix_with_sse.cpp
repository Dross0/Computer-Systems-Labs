#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <math.h>

#define N 1024
#define iter 5

float multiply(float *x, float *y)  //умножение векторов с SSE
{
    float sum;
    __m128 *xx, *yy;
    __m128 p, s;
    xx = (__m128 *) x;
    yy = (__m128 *) y;
    s = _mm_set_ps1(0); // устанавливает четыре значения, выровненные по адресу
    for (int i = 0; i < N / 4; i++) {
        p = _mm_mul_ps(xx[i], yy[i]); // векторное умножение 4 чисел
        s = _mm_add_ps(s, p); // векторная сумма
    }
    p = _mm_movehl_ps(p, s);  // перемещение 2-х старших значений s в младшие p
    s = _mm_add_ps(s, p); // сложение
    p = _mm_shuffle_ps(s, s, 1); // перемещение 2-го значения s в младшую позицию p
    s = _mm_add_ss(s, p); // сложение
    _mm_store_ss(&sum, s); // запись младшего значения в память
    return sum;
}

void dot(float A[], float B[], float C[]) // произведение матриц с SSE
{
    float *mas;
    mas = new float[N * N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            mas[j * N + i] = B[i * N + j];// транспанирование
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C[i * N + j] = multiply(&A[i * N], &mas[j * N]); // произведение
}

void oper(float A[], float B[], float C[], char oper) 
{
    __m128 p, s;
    for (int i = 0; i < N; i++) {
        __m128 *xx, *yy;
        xx = (__m128 * ) & A[i * N];
        yy = (__m128 * ) & B[i * N];
        for (int k = 0; k < N / 4; k++) {
	   if (oper == '+')
        	p = _mm_add_ps(xx[k], yy[k]); //сложение
	   if (oper=='-')
		p = _mm_sub_ps(xx[i], yy[k]); //вычитание
            _mm_store_ps(&C[i * N + k * 4], p); // записать четыре значения по выровненному адресу
        }
    }
}

void sub(float A[], float B[], float C[]) 
{
    oper(A, B, C, '-');
}

void add(float A[], float B[], float C[]){
    oper(A, B, C, '+');
}

double S_SSE(float mas[], float BB[]) {
    float *I, *B, *R, *C, *sib;
    float max1 = 0, maxst = 0;
    int summa = 0;
    clock_t start1;
    I = new float[N * N];
    B = new float[N * N];
    R = new float[N * N];
    C = new float[N * N];
    sib = new float[N * N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            summa += fabs(mas[i * N + j]);
        if (summa > max1)
            max1 = summa;
    }
    summa = 0;
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++)
            summa = summa + fabs(mas[i * N + j]);
        if (summa > maxst)
            maxst = summa;
    }
    for (int i = 0; i < N; i++)
        for (int j = i; j < N; j++) {
            B[(i) * N + (j)] = mas[(j) * N + (i)] / (max1 * maxst);
            B[(i) * N + (j)] = mas[(j) * N + (i)] / (max1 * maxst);
        }
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (i == j)
                I[i * N + j] = 512;
            else
                I[i * N + j] = 1;
    dot(mas, B, C);
    sub(I, C, R);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            BB[(i) * N + (j)] = I[i * N + j];
            sib[i * N + j] = I[i * N + j];
        }
    start1 = clock(); 
    for (int s = 1; s <= iter; s++) {
        dot(sib, R, C);
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                sib[i * N + j] = C[i * N + j];
        add(BB, sib, BB);
    }
    dot(BB, B, C);
    start1 = clock() - start1;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
           BB[i * N + j] = C[i * N + j];
    return (double) start1 / CLOCKS_PER_SEC; 
}

// программа без использования SSE

void umnogenie(float A[], float B[], float C[]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            C[i * N + j] = 0;
            for (int k = 0; k < N; k++)
                C[i * N + j] += A[i * N + k] * B[k * N + j];
        }
}

void sum(float A[], float B[], float C[]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C[i * N + j] = A[i * N + j] + B[i * N + j];
}

void raznost(float A[], float B[], float C[]) {
    int i, j;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C[i * N + j] = A[i * N + j] - B[i * N + j];
}

double flip(float mas[], float BB[]) {
    float *I, *B, *R, *C, *sib, max, maxst, summa;
    clock_t start;
    I = new float[N * N];
    B = new float[N * N];
    R = new float[N * N];
    C = new float[N * N];
    sib = new float[N * N];
    max = 0;
    summa = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            summa = summa + fabs(mas[i * N + j]);
        if (summa > max)
            max = summa;
    }
    maxst = 0;
    summa = 0;
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++)
            summa = summa + fabs(mas[i * N + j]);
        if (summa > maxst)
            maxst = summa;
    }
    for (int i = 0; i < N; i++)
        for (int j = i; j < N; j++) {
            B[i * N + j] = mas[j * N + i] / (max * maxst);
            B[i * N + j] = mas[j * N + i] / (max * maxst);
        }
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (i == j)
                I[i * N + j] = 1;
            else
                I[i * N + j] = 0;
    umnogenie(mas, B, C);
    raznost(I, C, R);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            BB[i * N + j] = I[i * N + j];
            sib[i * N + j] = I[i * N + j];
        }
    start = clock(); 
    for (int s = 1; s <= iter; s++) {
        umnogenie(sib, R, C);
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                sib[i * N + j] = C[i * N + j];
        sum(BB, sib, BB);
    }
    umnogenie(BB, B, C);
    start = clock() - start; 
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            BB[i * N + j] = C[i * N + j];
    return (double) start / CLOCKS_PER_SEC; 
}
using namespace std;
int main() {
    double t, t1, t2;
    clock_t start;
    float *m1, *tech;
    m1 = new float[N * N];
    tech = new float[N * N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            if (i == j)
                m1[i * N + j] = 512;
            else
                m1[i * N + j] = 1;
        }
    start = clock(); 
    t = flip(m1, tech);
    start = clock() - start; 
    t1 = (double) start / CLOCKS_PER_SEC;
    cout << endl << "without sse: " << t << endl;
    cout << "ALL without sse: " << t1 << endl << endl;
    start = clock();
    t = S_SSE(m1, tech);
    end = clock(); 
    t2 = (double) (end - start) / CLOCKS_PER_SEC;
    cout << "(S_SSE): " << t << endl;
    cout << "Alln(S_SSE): " << t2 << endl << endl;
    return 0;
}
