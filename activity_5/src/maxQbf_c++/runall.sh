rm -r out
mkdir out
make
for i in ../../instances/kqbf/kqbf*; do INST=$(echo $i | rev | cut --delimiter="/" -f 1 | rev); echo "Running $INST"; ./maxQbf_c++ < ../../instances/kqbf/$INST | tee -a out/$INST.out > /dev/null; done 
rm maxQbf