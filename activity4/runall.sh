rm -r out
mkdir out
g++ maxQbf.cpp -g -O3 -o maxQbf
for i in instances/kqbf*; do M=$(echo $i | rev | cut -d "/" -f 1 | rev); echo "Running instance $M"; ./maxQbf < "instances/$M" > "out/$M.out"; done
rm maxQbf