#include<omp.h>
#include<stdio.h>
#include<time.h>
#include<algorithm>
#include <math.h>
using namespace std;
void Init(size_t* A, size_t size);
void PrintArray(size_t* arr, size_t size);
size_t MaxElement(size_t* A, size_t* B, size_t size);
size_t MaxElementMP(size_t* A, size_t* B, size_t size);
size_t MaxElementMPLock(size_t* A, size_t* B, size_t size);
void UseBarrier(size_t* A, size_t* B, size_t size);
void main(void)
{
	srand(time(NULL));

	size_t size = 10000000, C, C_MP, C_MP_LOCK;
	size_t* A = new size_t[size];
	size_t* B = new size_t[size];

	double start;
	double end;


	//Check OpenMP working--------------------------------------
	size_t th_num, num_ths, max_th;
	max_th = omp_get_max_threads();
	printf_s("Max threads= %zu\n", max_th);

#pragma omp parallel num_threads(max_th) private (num_ths, th_num)
	{
		th_num = omp_get_thread_num();
		num_ths = omp_get_num_threads();
		printf("I am ready %zu from %zu \n", th_num, num_ths);
	}
	//end check-------------------------------------------------

	start = omp_get_wtime();
	Init(A, size);
	Init(B, size);
	end = omp_get_wtime();
	printf("\n\nWork took %f seconds to init\n\n", end - start);


	start = omp_get_wtime(); // start time counter
	C = MaxElement(A, B, size); // start func  (without OpenMP)
	end = omp_get_wtime(); // stop time counter
	printf_s("C = %zu\n", C); // print func`s result - summ array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds\n\n", end - start); // print time result

	start = omp_get_wtime(); // start time counter
	C_MP = MaxElementMP(A, B, size); // start func  (with OpenMP)
	end = omp_get_wtime(); // stop time counter
	printf_s("C_MP = %zu\n", C_MP); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds MP reduction\n\n", end - start); // print time result

	start = omp_get_wtime(); // start time counter
	C_MP_LOCK = MaxElementMPLock(A, B, size); // start func  
	end = omp_get_wtime(); // stop time counter
	printf_s("C_MP_Lock = %zu\n", C_MP_LOCK); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds MP Lock\n\n", end - start); // print time result


	start = omp_get_wtime(); // start time counter
	UseBarrier(A, B, size);
	end = omp_get_wtime(); // stop time counter
	printf("Work took %f seconds BARRIER\n\n", end - start); // print time result
	
	system("Pause");
	//return 1;
}

// init A[size]
void Init(size_t* A, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		A[i] = (rand() % 100);
	}
}

// find C[i]=Max(A[i],B[i]), return element`s sum. (without openmp)
size_t MaxElement(size_t* A, size_t* B, size_t size)
{
	size_t* C = new size_t[size];
	size_t sum = 0;
	for (size_t i = 0; i < size; i++)
	{
		sum += C[i] = max(A[i], B[i]);
	}
	return sum;
}

// find C[i]=Max(A[i],B[i]), return element`s sum. (with openmp reduction)
size_t MaxElementMP(size_t* A, size_t* B, size_t size){
	size_t* C = new size_t[size];
	size_t sum = 0;
	int i;
	size_t max_ = 0;
#pragma omp parallel shared(A,B,C)private(max_)
	{
#pragma omp for private(i) reduction(+:sum)
		for (i = 0; i < size; i++)
		{
			max_ = max(A[i], B[i]);
			C[i] = max_;
			sum = sum + max_;
		}
	}
	return sum;
}

size_t MaxElementMPLock(size_t* A, size_t* B, size_t size)
{
	size_t* C = new size_t[size];
	size_t sum = 0;
	int i;
	size_t max_ = 0;
	omp_lock_t lock;
	omp_init_lock(&lock);
#pragma omp parallel shared(A,B,C,sum)private(max_,i)
	{		
#pragma omp for 
		for (i = 0; i < size; i++)
		{
			max_ = max(A[i], B[i]);
			C[i] = max_;
			omp_set_lock(&lock);
			sum = sum+max_;
			omp_unset_lock(&lock);
		}
	}
	omp_destroy_lock(&lock);
	return sum;
}

void UseBarrier(size_t* A, size_t* B, size_t size)
{
	size_t* C = new size_t[size];
	size_t sum = 0;
	int i;
	size_t max_ = 0;
	omp_lock_t lock;
	omp_init_lock(&lock);
	
#pragma omp parallel shared(A,B,C,size,sum) private(max_,i)
	{
		Init(A, size);
		Init(B, size);
#pragma omp barrier
		
#pragma omp for 
		for (i = 0; i < size; i++)
		{
			max_ = max(A[i], B[i]);
			C[i] = max_;
			omp_set_lock(&lock);
			sum += max_;
			omp_unset_lock(&lock);
		}
		
	}
	omp_destroy_lock(&lock);
	printf_s("sum = %zu\n", sum);
}
