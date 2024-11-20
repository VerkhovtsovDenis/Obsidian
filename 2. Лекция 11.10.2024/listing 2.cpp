#include <iostream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <omp.h>

unsigned sum_omp_reduce(const unsigned* V, unsigned n, unsigned b, unsigned e) {
    unsigned sum = 0;
    #pragma omp parallel for reduction (+ :sum)
    for (int i = b; i < e; ++i)
        sum += V[i];
    return sum;
}


int main(){
	using std::cin;
    using std::cout;
    using std::endl;
    
    // Количество элементов в массиве
    unsigned n = 1u << 24;
    // Общее количество потоков
	unsigned T = omp_get_num_threads();

    // Количество элементов, над которые будут заняты парралельным процессом
    unsigned s = n / T;
    // Индекс 0-го элемента подпоследовательности [b; b+s) 
    unsigned b = n % T;


    #pragma omp parallel

    {
        // Номер текущего потока
        unsigned t = omp_get_thread_num();
        std::vector<unsigned> v(n);

        #pragma omp single
        {
            T = omp_get_num_threads();

            for(int i = 0; i < n; ++i)
                v[i] = i + T;
        }

        if (t < b)
            b = ++s * t;
        else
            b += s++;
        
        cout << "Thread " << t << "; b is " << b << endl; 

        unsigned e = b + s;
        unsigned answer = sum_omp_reduce(v.data(), v.size(), b, e);

        cout << "answer is " << answer << endl;
    }
    std::cin.get();

    return 0;
}