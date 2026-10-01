set -eux

cmake --build build

./build/evac

lli out.ll

# echo $?
