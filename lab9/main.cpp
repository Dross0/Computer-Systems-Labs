#include <iostream>
#include <fstream>

using ull = unsigned long long;

inline ull rdtsc(){
    unsigned int hi, lo;
    asm volatile ("rdtsc\n" : "=a" (lo), "=d" (hi));
    return ((ull)hi << 32) | lo;
}

double count_time(const int * arr, int size){
    ull min = -1;
    for (int j = 0; j < 10; ++j){
        ull start = rdtsc();
        for (int i = 0, k = 0; i < size; ++i){
            k = arr[k];
        }
        ull end = rdtsc();
        if (min > (end - start)){
            min = end - start;
        }
    }
    return static_cast<double>(min) / size;
}

int main() {
    const size_t CACHE_SIZE = 12582912;
    const int INT_COUNT_IN_CACHE = CACHE_SIZE / sizeof(int);
    double result[32] = {0};
    std::ofstream csv_file("out.csv");
    if (!csv_file.is_open()){
        throw std::runtime_error("Cant open csv file");
    }
    for (int fragments = 1; fragments <= 32; ++fragments){
        const int elements = INT_COUNT_IN_CACHE / fragments;
        const int size = INT_COUNT_IN_CACHE * fragments;
        int * array = new int[size];

        for (int i = 0; i < fragments; ++i){
            for (int j = 0; j < elements; ++j){
                array[i * INT_COUNT_IN_CACHE + j] = (i + 1) * INT_COUNT_IN_CACHE + j;
            }
        }

        for (int i = 0; i < elements; ++i){
            array[(fragments - 1) * INT_COUNT_IN_CACHE + i] = i + 1;
        }
        array[(fragments - 1) * INT_COUNT_IN_CACHE + (elements - 1)] = 0;

        result[fragments - 1] = count_time(array, size);

    }
    for (int i = 0; i < 32; ++i){
        csv_file << i + 1 << ";";
    }
    csv_file << std::endl;
    for (int i = 0; i < 32; ++i){
        csv_file << result[i] << ";";
    }


    return 0;
}