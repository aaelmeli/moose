call .\ci\setup.bat
cd build
set PATH=c:\vendors\cmake-ninja\bin;C:\Program Files (x86)\NSIS;%PATH%

call "C:\Program Files (x86)\Intel\Composer XE 2015\bin\ipsxe-comp-vars.bat" intel64 vs2013

cmake -DCMAKE_C_COMPILER=icl ^
      -DCMAKE_CXX_COMPILER=icl ^
	  -DBUILDNAME="Windows-Intel-15.0-Release-%CI_BUILD_REF_NAME%" ^
	  -DCMAKE_BUILD_TYPE:STRING=RELEASE ^
	  -Dwasp_ENABLE_TESTS:BOOL=ON ^
	  -Dwasp_ENABLE_ALL_PACKAGES:BOOL=ON ^
	  -DCMAKE_CXX_FLAGS:STRING=/FC ^
	  -G "Ninja" ^
	  ..
ctest -D ExperimentalStart ^
      -D ExperimentalBuild ^
	  -D ExperimentalTest ^
	  -D ExperimentalSubmit
cd ..\
rd /s /q build
mkdir build && cd build
cmake -DCPACK_GENERATOR=STGZ ^
	  -DCPACK_PACKAGE_NAME=WASP ^
	  -DCMAKE_C_COMPILER=icl -D CMAKE_CXX_COMPILER=icl ^
	  -DBUILDNAME="Windows-Intel-15.0-Bundle-%CI_BUILD_REF_NAME%" ^
	  -DCMAKE_BUILD_TYPE:STRING=RELEASE ^
	  -Dwasp_ENABLE_TESTS:BOOL=OFF ^
	  -Dwasp_ENABLE_ALL_PACKAGES:BOOL=ON ^
	  -Dwasp_ENABLE_testframework:BOOL=OFF ^
	  -Dwasp_ENABLE_googletest:BOOL=OFF ^
	  -Dwasp_ENABLE_wasppy:BOOL=ON ^
	  -Dwasp_ENABLE_INSTALL_CMAKE_CONFIG_FILES:BOOL=ON ^
	  -Dwasp_GENERATE_EXPORT_FILE_DEPENDENCIES:BOOL=ON ^
	  -Dwasp_ENABLE_CPACK_PACKAGING:BOOL=ON ^
	  -G "Ninja" ^
	  ..
REM need to fix the install
ninja package
copy /Y waspConfig_install.cmake install\lib\cmake\wasp\waspConfig.cmake
dir
REM Copy out of build directory so artifact logic will not have
REM build directory in ZIP
copy /Y WASP*.exe ..\
copy /Y waspConfig_install.cmake ..\
