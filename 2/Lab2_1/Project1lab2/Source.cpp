#include <omp.h>
#include<stdio.h>
#include<cstdlib>
#include<iostream>

const int NMAX = 20000;
const int LIMIT = 100000000;
void main()
{

	int th_num, num_ths, max_th;
	max_th = omp_get_max_threads();
	std::cout << "Max threads=" << max_th << "\n";

#pragma omp parallel num_threads(max_th) private (num_ths, th_num)
	{
		th_num = omp_get_thread_num();
		num_ths = omp_get_num_threads();
		printf("I am ready %d from %d \n", th_num, num_ths);
	}


	int i, j;
	float sum;
	//float a[NMAX][NMAX];
	float**a = new float*[NMAX];
	double start;
	double end;
	setlocale(LC_ALL, ("Russian"));






	for (i = 0; i < NMAX; i++)
	{
		a[i] = new float[NMAX];
		for (j = 0; j < NMAX; j++)
			a[i][j] = i + j;
	}

#pragma omp parallel shared(a) if (NMAX>LIMIT)
	{start = omp_get_wtime();
#pragma omp for private(i,j,sum) 
	for (i = 0; i < NMAX; i++)
	{
		sum = 0;
		for (j = 0; j < NMAX; j++)
			sum += a[i][j];
		//printf("Сумма элементов строки %d равна %f\n", i, sum);
	}
	end = omp_get_wtime();
	} /* Завершение параллельного фрагмента */

	printf("Work took %f seconds\n", end - start);
	system("Pause");

}
