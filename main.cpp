#include <vector>
#include <chrono>
#include <iostream>
#include <pthread.h>
#include <iomanip>

void calculate(unsigned long long size);
void *func(void *param);

std::vector<double> vectorA;
std::vector<double> vectorB;

unsigned long long vecSize;

const int threadNumber = 1;
//const int threadNumber = 2;
//const int threadNumber = 4;
//const int threadNumber = 8;
//const int threadNumber = 1000;

int main() {
    std::cout << "thread numbers: " << threadNumber << std::endl;
    std::cout << "calculating for 100000000 elements vector: ";
    calculate(100000000);
    std::cout << "calculating for 500000000 elements vector: ";
    calculate(500000000);
    std::cout << "calculating for 100000000000 elements vector: ";
    calculate(100000000000);
}

void calculate(unsigned long long size) {
    try {
        vectorA = std::vector<double>(size);
        vectorB = std::vector<double>(size);
        vecSize = size;
        double x = 0;
        unsigned long long i = 0;
        for (;i < size; ++i) {
            vectorA[i] = static_cast<double>(i) + 1;
            vectorB[i] = static_cast<double>(size - i) + 1;
            ++i;
        }

        pthread_t thread[threadNumber];
        int number[threadNumber];

        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < threadNumber; ++j) {
            number[j] = j;
            pthread_create(&thread[j], nullptr, func, (void*)(number+j));
        }

        double *temp_sum;
        for (int j = 0; j < threadNumber; ++j) {
            pthread_join(thread[j], (void **)&temp_sum);
            x += *temp_sum;
            delete temp_sum;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "it took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() <<
        "ms and X equals: " <<  std::setprecision(20) << x << "\n";
    }
    catch (const std::bad_alloc& e) {
        std::cout << "bad_alloc happend on vector with size " << size << "\n";
    }
}

void *func(void *param) {
    unsigned int shift = vecSize / threadNumber;
    int p = (*(unsigned long long*)param)*shift;
    double *sum = new double;
    *sum = 0;
    for (unsigned long long i = p; i < p+shift; ++i) {
        *sum += vectorA[i] * vectorB[i];
    }
    return (void*)sum;
}
