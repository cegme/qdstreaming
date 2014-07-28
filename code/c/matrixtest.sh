#! /usr/bin/bash

#rows=(10 20 30)
#cols=(10 20 30)
#distances=(0.1 0.5 0.9)
#samples=(10 100)
rows=(128 512 4096 32768 65536 131072 52488 1048576)
cols=(128 512 4096 32768 131072)
distances=(0.001 0.01 0.1 0.3 0.5 0.7 0.9 0.1)
samples=(1000 10000 100000 1000000)
matfile='/tmp/mymatrix.mat'


# Header
echo "row|col|sample|dist|memory(bytes)|matrixbuildtime(msecs)|sampletime(msecs)"

# Compile 
`make all`

for row in ${rows[@]}
do
	for col in ${cols[@]}
	do
		for sample in ${samples[@]}
		do
			for dist in ${distances[@]}
			do
			  # Remove the old matrix file
			  `rm $matfile`

			  # Memory before it is run
			  currentmem=`free -b | egrep Mem | awk '{print $4}'`

			  # Run with the current setting
			  runtime=`./a.out -d $dist -s $sample -c $col -r $row -f $matfile`

				# Print results
			  echo $row"|"$col"|"$sample"|"$dist"|"$currentmem"|"$runtime
			done
		done
	done
done


