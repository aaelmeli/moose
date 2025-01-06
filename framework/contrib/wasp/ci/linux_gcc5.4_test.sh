
export PATH=/projects/gcc-5.4.0/common_tools/anaconda3/bin/:$PATH
. /projects/gcc-5.4.0/load_dev_env_python3.sh

. ci/setup.sh
cd build
cmake -DBUILDNAME="$(uname -s)-GCC-5.4-Release-${CI_BUILD_REF_NAME}" \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -Dwasp_ENABLE_TESTS=ON \
      -DCMAKE_C_COMPILER=/projects/gcc-5.4.0/toolset/gcc-5.4.0/bin/gcc \
      -DCMAKE_CXX_COMPILER=/projects/gcc-5.4.0/toolset/gcc-5.4.0/bin/g++ \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -Dwasp_ENABLE_ALL_PACKAGES=ON \
      ..

ctest --output-on-failure \
      -D ExperimentalStart \
      -D ExperimentalBuild -j 8\
      -D ExperimentalSubmit \
      -D ExperimentalTest -j 8 \
      -D ExperimentalSubmit
