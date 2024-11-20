#include <iostream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <omp.h>
#include <stdlib.h>
#include <new>

#define N (1u << 28)

#define CACHE_LINE 64

#if defined (__GNUC__)&&__GNUC__<14
   #define hardware_destructive_interference_size 64
#else
   #include <thread>
   using std::hardware_destructive_interference_size
#endif

struct partial_sum_t {
    alignas(CACHE_LINE) unsigned val;
};

unsigned vector_sum_la(const unsigned* v, size_t n) {
    unsigned T;
    unsigned sum = 0;
    partial_sum_t* partial_sums;
    
    #pragma omp parallel
    {
        unsigned t = omp_get_thread_num();
    
        #pragma omp single
        {
            T = omp_get_num_threads();
            partial_sums = (partial_sum_t*)malloc(sizeof partial_sums[0] * T);
        }
        partial_sums[t].val = 0;
        unsigned s_t = n / T, b_t = n % T;

        if (t <= b_t)
            b_t = ++s_t * t;
        else
            b_t += s_t * t;

        unsigned e_t = b_t + s_t;

        for (unsigned i = b_t; i < e_t; i++)
            partial_sums[t].val += v[i];
    }

    for (unsigned i = 0; i < T; i++)
        sum += partial_sums[i].val;
    free(partial_sums);
    return sum;
}

unsigned sum_round_robin_aligned(const unsigned* v, unsigned n) {
    unsigned sum = 0;
    partial_sum_t* partial_sums;
    unsigned T;
    
    #pragma omp parallel
    {
        unsigned t = omp_get_thread_num();
        
        #pragma omp single
        {
            T = omp_get_num_threads();
            partial_sums = (partial_sum_t*)malloc(sizeof partial_sums[0] * T);
        }

        for (unsigned i = t; i < n; i += T)
            partial_sums[t].val += v[i];
    }

    for (unsigned i = 0; i < T; i++)
        sum += partial_sums[i].val;
    free(partial_sums);
    return sum;
}

unsigned sum_round_robin(const unsigned* v, unsigned n) {
    unsigned sum = 0;
    unsigned* partial_sums;
    unsigned T;
    #pragma omp parallel
    {
        T = omp_get_num_threads();
        unsigned t = omp_get_thread_num();
        #pragma omp single
        {
            partial_sums = (unsigned*) calloc(sizeof v[0], T);
        }

        for (unsigned i = t; i < n; i += T)
            partial_sums[t] += v[i];
    }

    for (unsigned i = 0; i < T; i++)
        sum += partial_sums[i];
    free(partial_sums);
    return sum;
}

unsigned sum_omp_reduce(const unsigned* v, unsigned n) {
    unsigned sum = 0;
    
    #pragma omp parallel for reduction(+ :sum)
    for (int i = 0; i < n; i++)
        sum += v[i];
    return sum;
}

unsigned sum_seq(const unsigned* v, unsigned n) {
    unsigned sum = 0;
    for (int i = 0; i < n; i++)
        sum += v[i];
    return sum;
}



int main(int argc, char** argv)  {

    auto V = std::make_unique<unsigned[]>(N);
    for (size_t i = 0; i < N; ++i)
        V[i] = i;

    auto t0 = omp_get_wtime();
    auto sum = sum_round_robin_aligned(V.get(), N);
    auto t1 = omp_get_wtime();
    std::cout << "Aligned:\t" << "0x" << std::hex << sum << "; \t" << (t1 - t0) * 1E+3 << std::endl;

    t0 = omp_get_wtime();
    auto sum1 = sum_seq(V.get(), N);
    t1 = omp_get_wtime();
    std::cout << "Posledovatelno:\t" << "0x" << std::hex << sum1 << "; \t" << (t1 - t0) * 1E+3 << std::endl;

    t0 = omp_get_wtime();
    auto sum2 = sum_round_robin(V.get(), N);
    t1 = omp_get_wtime();
    std::cout << "RR:\t\t" << "0x" << std::hex << sum2 << "; \t" << (t1 - t0) * 1E+3 << std::endl;;

    t0 = omp_get_wtime();
    auto sum3 = vector_sum_la(V.get(), N);
    t1 = omp_get_wtime();
    std::cout << "Vector:\t\t" << "0x" << std::hex << sum2 << "; \t" << (t1 - t0) * 1E+3;

    return 0;
}
