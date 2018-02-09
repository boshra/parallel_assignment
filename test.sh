declare -a COMM=("coll" "nonbl" "bl")
declare -a Ns=(50 150 250 500 1000 1500 2000 2500 3000 3500 4000)
FLAGS="-np 4 --bind-to core"
ulimit -s unlimited

for m in "${COMM[@]}"
do	
	for i in "${Ns[@]}"
	do
		for j in {1..10}
		do
			echo "mpirun $FLAGS $m $i >> results.csv"
			mpirun $FLAGS $m $i >> results.csv
		done
	done
done
