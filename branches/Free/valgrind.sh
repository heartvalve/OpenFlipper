#valgrind --log-file=valgrind.log --leak-check=full ./build-debug/Build/bin/OpenFlipper -c $@
#valgrind --tool=cachegrind --log-file=valgrind.log ./OpenFlipper/Debian_64_Debug/OpenFlipper $@
valgrind --tool=callgrind --log-file=valgrind.log ./build-debug/Build/bin/OpenFlipper  $@

