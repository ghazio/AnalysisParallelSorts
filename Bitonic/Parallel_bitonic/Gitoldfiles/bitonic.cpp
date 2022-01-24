/* C++ Program for Bitonic Sort. Note that this program
works only when size of input is a power of 2. */
#include<bits/stdc++.h>
using namespace std;

/*The parameter dir indicates the sorting direction, ASCENDING
or DESCENDING; if (a[i] > a[j]) agrees with the direction,
then a[i] and a[j] are interchanged.*/
void compAndSwap(int a[], int i, int j, int dir)
{
	if (dir==(a[i]>a[j]))
		swap(a[i],a[j]);
}

/*It recursively sorts a bitonic sequence in ascending order,
if dir = 1, and in descending order otherwise (means dir=0).
The sequence to be sorted starts at index position low,
the parameter cnt is the number of elements to be sorted.*/
void bitonicMerge(int a[], int low, int cnt, int dir)
{
	if (cnt>1)
	{
		int k = cnt/2;
		for (int i=low; i<low+k; i++)
			compAndSwap(a, i, i+k, dir);
		bitonicMerge(a, low, k, dir);
		bitonicMerge(a, low+k, k, dir);
	}
}

/* This function first produces a bitonic sequence by recursively
	sorting its two halves in opposite sorting orders, and then
	calls bitonicMerge to make them in the same order */
void bitonicSort(int a[],int low, int cnt, int dir)
{
	if (cnt>1)
	{
		int k = cnt/2;

		// sort in ascending order since dir here is 1
		bitonicSort(a, low, k, 1);

		// sort in descending order since dir here is 0
		bitonicSort(a, low+k, k, 0);

		// Will merge whole sequence in ascending order
		// since dir=1.
		bitonicMerge(a,low, cnt, dir);
	}
}

/* Caller of bitonicSort for sorting the entire array of
length N in ASCENDING order */
void sort(int a[], int N, int up)
{
	bitonicSort(a,0, N, up);
}

// A utility function to print an array
void print(int arr[], int n)
{
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
}

// Driver code
int main(int argc, char **argv){
   if (argc > 2) {
     printf("usage: ./bitonic sort (size) \n");
     exit(0);
    }
  //default: size of 100
    int size= 100;
    if (argc == 2) {
        size = atoll(argv[1]);
     }
   srand(0);
   int* randArray= (int*) malloc(sizeof(int)*size);
   for(int i=0;i<size;i++){
      randArray[i]=rand()%100;
      }
    print(randArray, size);
	int up = 1; // means sort in ascending order
	sort(randArray, size, up);
    print(randArray, size);	
	return 0;
}

