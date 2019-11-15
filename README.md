btree-project-cmake
==========================
This project is a C++ template project with cmake.


Requirements
-------------
The basic requirements for this example is a anaconda enviroment:


## Installation on LINUX/UNIX Systems

Download miniconda from

https://docs.conda.io/en/latest/miniconda.html

```
chmod +x Miniconda3-latest-Linux-x86_64.sh
bash Miniconda3-latest-Linux-x86_64.sh
source activate base
```

## Installation the following packages

```
conda install -c anaconda cmake
conda install -c conda-forge gtest
conda install -c conda-forge gmock
conda install -c hi2p-perim fmt
```

Note for osx:
` brew install fmt`


Build process
-------------
```
./conda/recipes/utecdf/build.sh
```

run gtest:
```
./btree-gtest
```

or 

```
cd /my_project_path/
mkdir build
cd build
cmake ..
make all
```
