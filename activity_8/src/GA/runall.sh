rm -r output
mkdir output
make
for i in ../../instances/kqbf/kqbf*; do INST=$(echo $i | rev | cut --delimiter="/" -f 1 | rev); echo "Running $INST"; ./GA < ../../instances/kqbf/$INST | tee -a output/$INST.ans > /dev/null; done 
rm maxQbf