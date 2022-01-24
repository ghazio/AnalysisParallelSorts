#!/bin/bash


# default hostfile
HOSTFILE=hostfile
WORKINGDIR=$PWD
OUTFILE1=$WORKINGDIR/resultsbigger__BITONICMPI.txt
echo $OUTFILE1
# optionally run with a hostfile command line arg
if [[ $# -gt 0 ]]; then
  HOSTFILE=$1
fi

echo "using hostfile $HOSTFILE"
echo "using hostfile $HOSTFILE" &>> $OUTFILE1


sleep 2

for((n=1024; n <= 524288; n=n*2))
#for((n=4; n <= 8; n=n*2))
do
for ((i=4; i <= 256; i=i*2))
#for ((i=16; i <= 32; i=i*2))
do
  echo " "
  echo "SIZE: $n, num_procs: $i"
  echo "SIZE: $n, num_procs: $i" &>>$OUTFILE1
  echo "time mpirun -np $i --hostfile $HOSTFILE ./bitonic  $n 0 $i"
  echo "time mpirun -np $i --hostfile $HOSTFILE ./bitonic  $n 0 $i" &>> $OUTFILE1
  #time mpirun -np $i --hostfile hostfile ./bitonic $n 1 $i
  #(time mpirun -np $i --hostfile hostfile ./bitonic $n 0 $i ;) 2>> $OUTFILE1
done
done
