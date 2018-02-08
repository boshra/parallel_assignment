declare -a COMM=("bl" "nonbl" "coll")
declare -a Ns=(500 1000 1500 2000 2500 3000 3500 4000)
FLAGS="--bind-to core"
ulimit -s unlimited

for m in "${COMM[@]}"
do	
	for i in "${Ns[@]}"
	do
		for j in {1..10}
		do
			echo "mpirun $FLAGS $m $i >> $m_result$i.txt"
			mpirun $FLAGS $m $i >> $m_result$i.txt
		done
	done
done
