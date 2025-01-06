. ci/setup.sh
cd build
module load valgrind
cmake -MEMCHECK_COMMAND="$(which valgrind)" \
      -DBUILDNAME="$(uname -s)-GCC-4.8.5-Debug-${CI_BUILD_REF_NAME}" \
      -DCMAKE_BUILD_TYPE=DEBUG \
      -Dwasp_ENABLE_COVERAGE_TESTING=ON \
      -DCOVERAGE_EXTRA_FLAGS="-s ${CI_PROJECT_DIR}/googletest -d" \
      -Dwasp_ENABLE_TESTS=ON \
      -Dwasp_ENABLE_ALL_PACKAGES=ON \
      ..



ctest --output-on-failure \
      -D Experimental -j 8 \
      -D ExperimentalMemCheck \
      -D ExperimentalCoverage \
      -D ExperimentalSubmit

