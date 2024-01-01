# Lab class exercises for INF585 Computer Animation

## Download



### Clone

```
git clone --recurse-submodules https://github.com/drohmer/inf585_lab_code.git
```

or

```
git clone https://github.com/drohmer/inf585_lab_code.git
cd inf585_code/
git submodule init
git submodule update
```

### Download archive

* [inf585_code.zip](https://imagecomputing.net/damien.rohmer/teaching/inf585/code/inf585_code.zip)
* [inf585_code.tar.gz](https://imagecomputing.net/damien.rohmer/teaching/inf585/code/inf585_code.tar.gz)


_Note: Do not use the "Code -> Download ZIP" option from the GitHub GUI as you won't get the CGP library as submodule with the code._


## Compilation

Scenes are in a directory _scenes_inf585/xx_Name/_

Each scene is provided with its own CMakeLists.txt and Makefile

Compilation via CMake
```
cd scenes_inf585/00_introduction/
cmake -B build/
cd build/
make -j8 # or any -jN, with N=$(nproc)
./00_introduction
```

Compilation via Makefile
```
cd scenes_inf585/00_introduction/
make -j8 # or any -jN, with N=$(nproc)
./00_introduction
```

More details are provided in the dedicated [Installation setup and Compilation page](https://imagecomputing.net/cgp/compilation) if needed (Windows, Linux, Mac).

## Requirement

The code requires the GLFW library in Linux and Mac.

ex. on Ubuntu

```
sudo apt-get install libglfw3-dev
```

For Mac:
* You may use brew to install glfw. And add the following two lines on your .zshrc to add glfw library to the path
```
export CPATH=/opt/homebrew/include
export LIBRARY_PATH=/opt/homebrew/lib
```
* Alternatively, a pre-compiled glfw can be used without installation. To activate it, open the CMakeLists.txt file and set the option MACOS_GLFW_PRECOMPILED to ON. You may need to explicitely allow the mac to use the library if security triggers an alert.


## Links

* [Lab class exercises](https://damienrohmer.com/data/teaching/2022_2023/x-inf585/practice/index.html)
* [Documentation on CGP library](https://imagecomputing.net/cgp/index.html)


* [INF585 Course Website](https://damienrohmer.com/data/teaching/2022_2023/x-inf585)

### Open source content

* [Source lab class content](https://github.com/drohmer/inf585_lab_class)
* [Source slides content](https://github.com/drohmer/inf585_course_slides)
* [Source CGP library](https://github.com/drohmer/cgp)
