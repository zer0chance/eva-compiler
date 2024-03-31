set -ux

cmake --build . --clean-first -v

./evac

lli out.ll

echo $?

