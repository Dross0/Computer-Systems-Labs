#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int factorial(const int n){
    int res = 1;
    for (size_t i = 1; i <= n; ++i){
        res *= i;
    }
    return res;
}

double power(double a, int n){
    if (n == 0)
        return 1;
    if (n % 2 == 0)
        return power(a * a, n / 2);
    return a * power(a, n - 1);
}

double epower(const long long int n, const long long int x){
    double epow = 0;
    double last = 0;
    double tmp = 0;
    for(long int i = 0; i <= n; ++i){
        if (i <= 7){
            last = power(x, i) / factorial(i);
            epow += last;
        }
        else{
            last *= (double)((double)x / i);
            epow += last;

        }
    }
    return epow;
}


int main(int argc, char ** argv) {
    if (argc != 3){
        return 1;
    }
    long long int n = atoll(argv[1]);
    long long int x = atoll(argv[2]);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    double epow = epower(n, x);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    printf("Time = %lf \n", end.tv_sec - start.tv_sec + 0.000000001 * (end.tv_nsec - start.tv_nsec));
    printf("e^%lld = %.15lf\n", x, epow);
    return 0;
}