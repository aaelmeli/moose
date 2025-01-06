 which git
 module load cmake
 which cmake
 which python
 python --version
if [[ -d build  ]]; then
   rm -rf build
   mkdir build
else
   mkdir build
fi
 env
