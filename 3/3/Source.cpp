#include<omp.h>
#include<stdio.h>
#include<iostream>
#include<time.h>
#include<vector>
#include<algorithm> 
using namespace std;
void Init(size_t* A, size_t size);
void PrintArray(size_t*arr, size_t size);
size_t MaxElement(size_t* A, size_t* B, size_t size);
size_t MaxElementMP(size_t* A, size_t* B, size_t size);
void main() {

	srand(time(NULL));
	
	size_t size = 10,C, C_MP;
	size_t* A = new size_t[size];
	size_t * B = new size_t[size];

	double start;
	double end;


	//�������� ������ �������--------------------------------------
	size_t th_num, num_ths, max_th;
	max_th = omp_get_max_threads();
	printf_s("Max threads= %zu\n",max_th) ;

#pragma omp parallel num_threads(max_th) private (num_ths, th_num)
	{
		th_num = omp_get_thread_num();
		num_ths = omp_get_num_threads();
		printf("I am ready %zu from %zu \n", th_num, num_ths);
	}
	//end check-------------------------------------------------

	Init(A,size);
	Init(B,size);

	start = omp_get_wtime();			// ������ ������� �������
	C = MaxElement(A,B,size);		   // ������ ������� ���������� ��������� (���������� ��� ���������������)
	end = omp_get_wtime();			  // ��������� ������� �������
	printf_s("C = %zu\n", C);		 // ����� ���������� ������� - ����� ��������� ������� �, ��� C[i]=Max(A[i],B[i])
	printf("Work took %f seconds\n", end - start); // ����� ������������ �������


	start = omp_get_wtime();		   // ������ ������� �������
	C_MP = MaxElementMP(A, B,size);   // ������ ������� ���������� ��������� (���������� c �������������� OpenMP)
	end = omp_get_wtime();			 // ��������� ������� �������
	printf_s("C_MP = %zu\n", C_MP); // ����� ���������� ������� - ����� ��������� ������� �, ��� C[i]=Max(A[i],B[i])
	printf("Work took %f seconds MP\n", end - start);// ����� ������������ �������
	

	system("Pause");
}

// ��������� ������ � �������� size ���������� �������
void Init(size_t* A, size_t size) { 
	for (size_t i = 0; i < size; i++)
	{
		A[i] = (rand() % 100);
	}
}

// ������� C[i]=Max(A[i],B[i]), ���������� ����� ��������� �. (��� OpenMP)
size_t MaxElement(size_t* A, size_t* B,size_t size) {
	size_t *C=new size_t[size];
	size_t sum = 0;
	for (size_t i = 0; i < size; i++)
	{
		sum += C[i] = max(A[i], B[i]);
	}	
	//PrintArray(C, size);
	return sum;
}

// ������� ���������� ������� arr �������� size
void PrintArray(size_t*arr,size_t size) {
	printf_s("\n");
	for (size_t i = 0; i < size ; i++)
	{
		printf_s("%zu ", arr[i]);
	}
	printf_s("\n");
}

// ������� C[i]=Max(A[i],B[i]), ���������� ����� ��������� �. (� OpenMP)
 size_t MaxElementMP(size_t* A, size_t* B,size_t size) {
	
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