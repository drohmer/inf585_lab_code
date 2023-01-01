# Lab class exercises for INF585 Computer Animation

## Download



### Clone

```
git clone --recurse-submodules https://github.com/drohmer/inf585_code.git
```

or

```
git clone https://github.com/drohmer/inf585_code.git
cd inf585_code/
git submodule init
git submodule update
```

### Download archive

* - [inf585_code.zip](https://imagecomputing.net/damien.rohmer/teaching/inf585/code/inf585_code.zip)
* - [inf585_code.tar.gz](https://imagecomputing.net/damien.rohmer/teaching/inf585/code/inf585_code.tar.gz)


_Note: Do not use the "Code -> Download ZIP" option from the GitHub GUI as you won't get the CGP library as submodule with the code._


## Compilation

Scenes are in a directory _scenes_inf585/xx_Name/_

Each scene is provided with its own CMakeLists.txt and Makefile

Compilation via CMake
```
cd scenes_inf585/00_introduction/
cmake -B build/
cd build/
make -j$(nproc)
./00_introduction
```

Compilation via Makefile
```
cd scenes_inf585/00_introduction/
make -j$(nproc)
./00_introduction
```

More details are provided in the dedicated [Installation setup and Compilation page](https://imagecomputing.net/cgp/compilation) if needed (Windows, Linux, Mac).

## Requirement

The code requires the GLFW library in Linux and Mac.

ex. on Ubuntu

```
sudo apt-get install libglfw3-dev
```



## Links

* [Lab class exercices](https://imagecomputing.net/damien.rohmer/teaching/inf585/practice/index.html)
* [Documentation on CGP library](https://imagecomputing.net/cgp/index.html)



