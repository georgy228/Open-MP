# Отчет о выполнении лабораторной работы №5
## Замки и барьеры
[Код программы](https://github.com/sekibura/Open-MP/blob/master/6/6/Source.cpp)
### Задание 1
Модифицировать программы, составленные в Л.Р. №№3,4, используя переменные семафорного типа.
Измерять время работы программы для тех же значений параметров, что были использованы при выполнении Л.Р. №3. Результаты сравнить и занести в отчёт.
#### Выполнение:
Семафоры я буду использовать в функции, которая совершает основную работу - выделяет максимум и складывает значения всемх максимумов.

Ее реализация с использованием редукции:
```
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
```

Уберем редукцию и добавим lock`и в месте доступа к общей переменной sum, в которую суммируются максимальные элементы:
```
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
```
------
### Задание 2
Модифицировать программы, составленные в Л.Р. №№3,4, используя методы барьерной синхронизации.э
Измерять время работы программы для тех же значений параметров, что были использованы при выполнении Л.Р. №3. Результаты сравнить и занести в отчёт.
#### Выполнение:
Для органиченого внедрения барьеров, я вынес всю суть программы в одну функцию:
```
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
```
Сначала организую инициализацию массивов A и B. Т.к. дальнейшая работа требует оба массива в инициализированном виде, ставлю барьер - программа не начнет работу с массивами, пока 
они не будут инициализированы. Дальше внедрен код с семафором из предыдущей функции.

------
### Таблица времен выполнения:

|  |  100 | 10 000  | 100 000| 1 000 000| 10 000 000|
|:--------:|---|---| ---|---|---|
| Init   |  0.000063 | 0.006023  | 0.052976   |  0.344536   |  2.900233 | 
| Max  | 0.000016 |  0.000711 |  0.006984   | 0.047132  | 0.396382  | 
| Max(MP reduction)  | 0.000282 | 0.000229  |  0.001764   | 0.011505   |  0.065677 |     
| Max Lock | 0.000085  | 0.005664  |   0.066564   |  0.607432 |  6.770948 |
| Barrier |  0.001165 |  0.015261 |0.144358  |  1.244296 | 11.860955  |

Пример работы программы для 10 000 000 элементов:

![image](https://user-images.githubusercontent.com/51335422/100235826-ecf63100-2f3d-11eb-948f-939dfb84aafa.png)

