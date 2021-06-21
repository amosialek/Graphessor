# Graphessor
Image commpression using graph grammars

Built with BOOST 1.76

## Installation
the project has been built with following setup on Ubuntu 20.04:
 
 - g++ 9.3.0
 - make 4.2.1
 - cmake 3.16.3
 - boost 1.76.0
 - cereal https://github.com/USCiLab/cereal/commit/46a4a910077bf9e9f8327c8f6ea761c89b06da53
 - spdlog https://github.com/gabime/spdlog/commit/12df172575bf43ed6b879dabff4ba59c72e738e3
 - liblapacke-dev 3.9.0-1build1

Assuming, you have already installed git, g++, make and cmake, here are installation commands I use, to install other dependencies on Ubuntu. The project should compile with newer versions of libs as well

Download and unpack boost from https://www.boost.org/users/history/version_1_76_0.html
```
cd boost_1_76_0/
sudo ./bootstrap.sh
sudo ./b2 install
cd ..

```
```
sudo apt install liblapacke-dev
mkdir utils
cd utils/
git clone git@github.com:gabime/spdlog.git
cd spdlog/
git checkout 12df172575bf43ed6b879dabff4ba59c72e738e3
mkdir build && cd build
cmake ..
make -j
sudo make install
cd ../..
git clone git@github.com:USCiLab/cereal.git
cd cereal/
git checkout 46a4a910077bf9e9f8327c8f6ea761c89b06da53
sudo apt-get install gcc-multilib g++-multilib
cmake .
sudo make install
```

now go to the project's directory and run
```
cmake .
make
```
executable should appear in src/executable/

# Running 

You can run the project with following line
```
./src/executable/Graphessor
--input path-to-bmp-file 
--epsilon max-error-to-be-tolerated
--output-directory path
--output-file-template filename
--order order-of-polynomials  
--graph-output output-path-for-graphs
```
where epsilon is double precision number, order is an integer and other parameters are strings. The output directory will be created if it does not exist. 