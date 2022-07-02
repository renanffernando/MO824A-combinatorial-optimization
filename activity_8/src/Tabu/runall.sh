rm -r output
mkdir output
make
for i in ../../instances/kqbf/kqbf*; do INST=$(echo $i | rev | cut --delimiter="/" -f 1 | rev); echo "Running $INST"; for h in {1..1}; do ./Tabu < ../../instances/kqbf/$INST | tee -a output/$INST.ans > /dev/null; done; done 
rm maxQbf