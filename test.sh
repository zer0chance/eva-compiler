set -eux

cmake --build .

./evac

lli out.ll

# echo $?

