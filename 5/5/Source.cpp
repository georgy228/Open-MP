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
size_t MaxElementSections(size_t* A, size_t* B, size_t size);
size_t MaxElement2Sections(size_t* A, size_t* B, size_t size);
size_t MaxElement4Sections(size_t* A, size_t* B, size_t size);
size_t MaxElement8Sections(size_t* A, size_t* B, size_t size);

void main()
{
	srand(time(NULL));

	size_t size = 11000000, C, C_MP;
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
	printf("\n\nWork took %f seconds to init\n", end - start);

	//first way use sections. approximate difference ~ 0.1 sec
	start = omp_get_wtime();
#pragma omp sections
	{
#pragma omp section
		Init(A, size);
#pragma omp section
		Init(B, size);
	}
	end = omp_get_wtime();
	printf("Work took %f seconds to init (SECTIONS)\n\n", end - start);


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
	C = MaxElementSections(A, B, size); // start func  (with Sections inside for )
	end = omp_get_wtime(); // stop time counter
	printf_s("C = %zu\n", C); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds SECTIONS inside for loop \n\n", end - start); // print time result


	start = omp_get_wtime(); // start time counter
	C = MaxElement2Sections(A, B, size); // start func  (with 2 sections)
	end = omp_get_wtime(); // stop time counter
	printf_s("C = %zu\n", C); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds  with 2 SECTIONS \n\n", end - start); // print time result

	start = omp_get_wtime(); // start time counter
	C = MaxElement4Sections(A, B, size); // start func  (with 4 sections)
	end = omp_get_wtime(); // stop time counter
	printf_s("C = %zu\n", C); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds  with 4 SECTIONS \n\n", end - start); // print time result

	start = omp_get_wtime(); // start time counter
	C = MaxElement8Sections(A, B, size); // start func  (with 8 sections)
	end = omp_get_wtime(); // stop time counter
	printf_s("C = %zu\n", C); // print func`s result - sum array`s elements, where C[i]=Max(A[i],B[i])
	printf("Work took %f seconds  with 8 SECTIONS \n\n", end - start); // print time result

	system("Pause");
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
	//PrintArray(C, size);
	return sum;
}

size_t MaxElementSections(size_t* A, size_t* B, size_t size)
{
	size_t* C = new size_t[size];
	size_t sum = 0;
	size_t max_ = 0;

	for (size_t i = 0; i < size; i++)
	{
#pragma omp sections
		{
			max_ = max(A[i], B[i]);
#pragma omp section
			C[i] = max_;
#pragma omp section
			sum += max_;
		}
	}


	//PrintArray(C, size);
	return sum;
}

// print array
void PrintArray(size_t* arr, size_t size)
{
	printf_s("\n");
	for (size_t i = 0; i < size; i++)
	{
		printf_s("%zu ", arr[i]);
	}
	printf_s("\n");
}

// find C[i]=Max(A[i],B[i]), return element`s sum. (with openmp reduction)
size_t MaxElementMP(size_t* A, size_t* B, size_t size)
{
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
	//PrintArray(C, size);
	return sum;
}

size_t MaxElement2Sections(size_t* A, size_t* B, size_t size)
{
	size_t* C = new size_t[size];
	size_t sum = 0;
	size_t max_ = 0;
# pragma  omp parallel 
	{
#pragma omp sections reduction(+:sum)private(max_)
		{
#pragma omp section
			{
				for (size_t i = 0; i < round(size / 2); i++)
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum = sum + max_;
				}
			}
#pragma omp section
			{
				for (size_t i = round(size / 2); i < size; i++)
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum = sum + max_;
				}
			}
		}
	}
	//PrintArray(C, size);
	return sum;
}

size_t MaxElement4Sections(size_t* A, size_t* B, size_t size)
{
	size_t* C = new size_t[size];
	size_t sum = 0;
	size_t max_ = 0;
# pragma  omp parallel
	{
#pragma omp sections reduction(+:sum)private(max_)
		{
#pragma omp section
			{
				for (size_t i = 0; i < round(size / 4); i++) // 0-1/4
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}
#pragma omp section
			{
				for (size_t i = round(size / 4); i < round(size / 2); i++) // 1/4-2/4
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}

#pragma omp section
			{
				for (size_t i = round(size / 2); i < 3 * round(size / 4); i++) // 2/4 - 3/4
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}

#pragma omp section
			{
				for (size_t i = 3 * round(size / 4); i < size; i++) // 3/4 - 1
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}
		}
	}
	//PrintArray(C, size);
	return sum;
}

size_t MaxElement8Sections(size_t* A, size_t* B, size_t size)
{
	size_t* C = new size_t[size];
	size_t sum = 0;
	size_t max_ = 0;
# pragma  omp parallel
	{
#pragma omp sections reduction(+:sum)private(max_)
		{
#pragma omp section
			{
				for (size_t i = 0; i < round(size / 8); i++) // 0-1/8
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}
#pragma omp section
			{
				for (size_t i = round(size / 8); i < round(size / 4); i++) // 1/8- 2/8
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}

#pragma omp section
			{
				for (size_t i = round(size / 4); i < 3 * round(size / 8); i++) // 2/8 - 3/8
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}

#pragma omp section
			{
				for (size_t i = 3 * round(size / 8); i < round(size / 2); i++) // 3/8-4/8
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}
#pragma omp section
			{
				for (size_t i = round(size / 2); i < 5 * round(size / 8); i++) // 4/8-5/8
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}
#pragma omp section
			{
				for (size_t i = 5 * round(size / 8); i < 6 * round(size / 8); i++) // 5/8-6/8
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}
#pragma omp section
			{
				for (size_t i = 6 * round(size / 8); i < 7 * round(size / 8); i++) // 6/8-7/8
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}
#pragma omp section
			{
				for (size_t i = 7 * round(size / 8); i < size; i++) //7/8-1
				{
					max_ = max(A[i], B[i]);
					C[i] = max_;
					sum += max_;
				}
			}
		}
	}
	//PrintArray(C, size);
	return sum;
}
