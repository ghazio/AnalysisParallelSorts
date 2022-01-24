
#include <mpi.h>



//compare low function recieves the minimum from paired processor, sends its maximum,
//and makes new bigger bitonic sequence
void CompareLow(int j);
int ComparisonFunc(const void * a, const void * b);
//compare high function recieves the maximum from paired processor, sends it minimum,
//and makes a new bitonic sequence with the paried processor
void CompareHigh(int j);
