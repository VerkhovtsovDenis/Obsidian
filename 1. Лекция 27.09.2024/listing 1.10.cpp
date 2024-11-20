#include <iostream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <omp.h>

unsigned sum(const unsigned* V, unsigned n) {
    unsigned sum = 0;
    for (int i = 0; i < n; i++) 
        sum += V[i];
    return sum;
}

unsigned sum_omp_reduce(const unsigned* V, unsigned n) {
    unsigned sum = 0;
    #pragma omp parallel for reduction (+ :sum)
    for (int i = 0; i < n; i++)
        sum += V[i];
    return sum;
}

unsigned sum_round_robin(const unsigned* V, unsigned n)
{
    unsigned sum = 0;
    unsigned* partial_sums;
    unsigned T;

    #pragma omp parallel

    {
        unsigned t = omp_get_thread_num();
        #pragma omp single
        {
            T = omp_get_num_threads();
            partial_sums = (unsigned*)calloc(sizeof V[0], T);
        }
        for (unsigned i = t; i < n; i += T)
            partial_sums[t] += V[i];
        sum = partial_sums[0];
        
        for (unsigned i = 1; i < T; ++i)
            sum += partial_sums[i];
    }
    
    free(partial_sums);
    
    return sum;
}

int main()
{
    std::vector<unsigned> v(1u << 24, 12);

    std::cout << "sum(v, n)=" << std::hex << sum(v.data(), v.size()) << "\n";
    std::cout << "sum_omp_reduce(v, n)=" << std::hex << sum_omp_reduce(v.data(), v.size()) << "\n";
    std::cout << "sum_round_robin(v, n)=" << std::hex << sum_round_robin(v.data(), v.size()) << "\n";
    
    std::cin.get();
    
    return 0;
}