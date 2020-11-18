#include<omp.h>
//#include<stdio.h>
#include<iostream>
#include<ctime>
#include<algorithm> 
using namespace std;
void Init(size_t* A, size_t size);
void PrintArray(size_t*arr, size_t size);
size_t MaxElement(size_t* A, size_t* B, size_t size);
size_t MaxElementMPAtomic(size_t* A, size_t* B, size_t size);
size_t MaxElementMPCritical(size_t* A, size_t* B, size_t size);
size_t MaxElementMPReduction(size_t* A, size_t* B, size_t size);
void main() {

	srand(time(NULL));

	size_t size = 10000000, C, C_MP;
	size_t* A = new size_t[size];
	size_t * B = new size_t[size];

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

	Init(A, size);
	Init(B, size);

	start = omp_get_wtime();			// start time counter
	C = MaxElement(A, B, size);		   // start func  (without OpenMP)
	end = omp_get_wtime();			  // stop time counter
	printf_s("C = %zu\n", C);		 // print func`s result - summ array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds\n", end - start); // print time result


	start = omp_get_wtime();		   // start time counter
	C_MP = MaxElementMPAtomic(A, B, size);   // start func  (with OpenMP Atomic)
	end = omp_get_wtime();			 // stop time counter
	printf_s("C_MP Atomic = %zu\n", C_MP); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds MP\n", end - start);// print time result

	start = omp_get_wtime();		   // start time counter
	C_MP = MaxElementMPCritical(A, B, size);   // start func  (with OpenMP Critical)
	end = omp_get_wtime();			 // stop time counter
	printf_s("C_MP Critical = %zu\n", C_MP); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds MP\n", end - start);// print time result

	start = omp_get_wtime();		   // start time counter
	C_MP = MaxElementMPReduction(A, B, size);   // start func  (with OpenMP Reduction)
	end = omp_get_wtime();			 // stop time counter
	printf_s("C_MP Reduction = %zu\n", C_MP); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds MP\n", end - start);// print time result

	system("Pause");
}

// init A[size]
void Init(size_t* A, size_t size) {
	for (size_t i = 0; i < size; i++)
	{
		A[i] = (rand() % 100);
	}
}

// find C[i]=Max(A[i],B[i]), return element`s sum. (without openmp)
size_t MaxElement(size_t* A, size_t* B, size_t size) {
	size_t *C = new size_t[size];
	size_t sum = 0;
	for (size_t i = 0; i < size; i++)
	{
		sum += C[i] = max(A[i], B[i]);
	}
	//PrintArray(C, size);
	return sum;
}

// print array
void PrintArray(size_t*arr, size_t size) {
	printf_s("\n");
	for (size_t i = 0; i < size; i++)
	{
		printf_s("%zu ", arr[i]);
	}
	printf_s("\n");
}

// find C[i]=Max(A[i],B[i]), return element`s sum. (with openmp)
size_t MaxElementMPAtomic(size_t* A, size_t* B, size_t size) {

	size_t* C = new size_t[size];
	size_t sum = 0;
	int i;
	size_t max_ = 0;


#pragma omp parallel shared(A,B,C,sum)private(max_)
	{
#pragma omp for private(i) /*reduction(+:sum)*/
		for (i = 0; i < size; i++)
		{
			max_ = max(A[i], B[i]);
			C[i] = max_;
#pragma omp atomic
			sum +=max_;
		}
	}
	//PrintArray(C, size);
	return sum;
}

size_t MaxElementMPCritical(size_t* A, size_t* B, size_t size) {

	size_t* C = new size_t[size];
	size_t sum = 0;
	int i;
	size_t max_ = 0;


#pragma omp parallel shared(A,B,C,sum)private(max_)
	{
#pragma omp for private(i) /*reduction(+:sum)*/
		for (i = 0; i < size; i++)
		{
			max_ = max(A[i], B[i]);
			C[i] = max_;
#pragma omp critical
			sum += max_;
		}
	}
	//PrintArray(C, size);
	return sum;
}

size_t MaxElementMPReduction(size_t* A, size_t* B, size_t size) {

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

			sum += max_;
		}
	}
	//PrintArray(C, size);
	return sum;
}