#include <iostream>
#include <limits>
#include <fstream>


using ull = unsigned long long;

inline ull rdtsc() {
    unsigned int lo, hi;
    asm volatile ( "rdtsc\n" : "=a" (lo), "=d" (hi) );
    return ((ull)hi << 32) | lo;
}

void direct_pass_fill(int * arr, const int N){
    for (int i = 0; i < N; ++i){
        arr[i] = i + 1;
    }
    arr[N-1] = 0;
}

void return_pass_fill(int *arr, const int N){
    for (int i = N-1; i > 0; --i){
        arr[i] = i - 1;
    }
    arr[0] = N-1;
}

void random_pass_fill(int * arr, const int N){
    direct_pass_fill(arr, N);
    srand(time(NULL));
    for (int i = N-1; i >= 1; --i){  //Алгоритм Фишера – Йетса
        int j = rand() % (i+1);

        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

double count_time(const int * arr, const int size){
    for (int i = 0, k =0; i < size; ++i){
        k = arr[k];
    }
    ull min_count = std::numeric_limits<ull>::max();
    for (int j = 0; j < 10; ++j){
        ull start = rdtsc();
        for (int i = 0, k = 0; i < size; ++i){
            k = arr[k];
        }
        ull end = rdtsc();
        if (end - start < min_count){
            min_count = end - start;
        }
    }
    return static_cast<double>(min_count) / size;
}

template <class T>
void print_csv_array(std::ofstream& csv_file, const T * arr, int size){
    for (int i = 0; i < size; ++i){
        csv_file << arr[i] << ';';
    }
    csv_file << std::endl;
}



int main() {
    const int NUMBER_OF_POINTS = 100;
    double k = 0;
    size_t size = 256;
    size_t sizes[100];
    double direct[100];
    double reverse[100];
    double random[100];
    for (int i = 0; i < NUMBER_OF_POINTS; ++i){
        int *arr = new int[size];
        sizes[i] = size;
        direct_pass_fill(arr, size);
        direct[i] = count_time(arr, size);
        return_pass_fill(arr, size);
        reverse[i] = count_time(arr, size);
        random_pass_fill(arr, size);
        random[i] = count_time(arr, size);
        size = static_cast<size_t>(size * (1.25 - k));
        k += 0.0025;
        delete [] arr;
        std::cout << i << std::endl;
    }
    std::ofstream csv_file("out.csv");
    if (!csv_file.is_open()){
        throw std::runtime_error("Cant open csv file");
    }
    csv_file.setf(std::ios_base::fixed);
    csv_file.precision(2);
    print_csv_array(csv_file, sizes, NUMBER_OF_POINTS);
    print_csv_array(csv_file, direct, NUMBER_OF_POINTS);
    print_csv_array(csv_file, reverse, NUMBER_OF_POINTS);
    print_csv_array(csv_file, random, NUMBER_OF_POINTS);
    return 0;
}