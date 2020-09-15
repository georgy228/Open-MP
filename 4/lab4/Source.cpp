#include<omp.h>
#include<stdio.h>
#include<iostream>
#include<time.h>
#include<vector>
#include<algorithm> 
using namespace std;
void Init(vector<size_t>* A);
void PrintVector(vector<size_t> vec);
vector<size_t> MaxElement(vector<size_t>* A, vector<size_t>* B);
//vector<size_t> MaxElementMP(vector<size_t>* A, vector<size_t>* B);
void MaxElementMP(vector<size_t>* A, vector<size_t>* B);
void main() {

	srand(time(NULL));

	size_t size = 50000000;
	vector<size_t> A(size), B(size), C, C_MP;

	double start;
	double end;


	//check threads working--------------------------------------
	int th_num, num_ths, max_th;
	max_th = omp_get_max_threads();
	cout << "Max threads=" << max_th << "\n";

#pragma omp parallel num_threads(max_th) private (num_ths, th_num)
	{
		th_num = omp_get_thread_num();
		num_ths = omp_get_num_threads();
		printf("I am ready %d from %d \n", th_num, num_ths);
	}
	//end check-------------------------------------------------

	Init(&A);
	Init(&B);
	//PrintVector(A);
	//PrintVector(B);

	start = omp_get_wtime();
	C = MaxElement(&A, &B);
	end = omp_get_wtime();
	//PrintVector(C);
	printf("Work took %f seconds\n", end - start);

	start = omp_get_wtime();
	//C_MP = MaxElementMP(&A, &B);
	MaxElementMP(&A, &B);
	end = omp_get_wtime();
	//PrintVector(C_MP);
	printf("Work took %f seconds MP\n", end - start);
	system("Pause");
}

void Init(vector<size_t>* A) {
	for (size_t i = 0; i < (*A).size(); i++)
	{
		(*A).at(i) = (rand() % 100);
	}
}

vector<size_t> MaxElement(vector<size_t>* A, vector<size_t>* B) {
	vector<size_t> result;
	size_t sum = 0;
	if ((*A).size() == (*B).size()) {
		result.resize((*A).size());

		for (size_t i = 0; i < (*A).size(); i++)
		{
			sum += result[i] = max((*A)[i], (*B)[i]);

		}
	}
	cout << "\nsum = " << sum << "\n";
	return result;
}

void PrintVector(vector<size_t> vec) {
	cout << "\n";
	for (size_t i : vec)
		cout << i << ' ';
	cout << "\n";
}

/*vector<size_t>*/void MaxElementMP(vector<size_t>* A, vector<size_t>* B) {
	vector<size_t> result;
	size_t sum = 0;
	int i;
	int max_ = 0;
	if ((*A).size() == (*B).size()) {
		//result.resize((*A).size());
#pragma omp parallel shared(A,B,sum) private(result,max_)
		{
#pragma omp for private(i) 
			for (i = 0; i < (*A).size(); i++)
			{
				/*result[i] = max((*A)[i], (*B)[i]);*/
				max_ = max((*A)[i], (*B)[i]);
				result.push_back(max_);
				#pragma omp atomic
				sum +=max_;
			}
			//PrintVector(result);
		}
	}
	cout << "\nsum MP = " << sum << "\n";
	/*return result;*/
}