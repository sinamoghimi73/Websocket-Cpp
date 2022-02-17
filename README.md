# Bitcoin Websocket C++


## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.3 (Linux, macOS)
  * Linux: make is installed by default on most Linux distros
  * macOS: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
* gcc/g++ >= 6
  * Linux: gcc / g++ is installed by default on most Linux distros
  * macOS: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
* boost >= 1.76.0 (Linux, macOS)
    * Linux: `sudo apt-get install libboost-all-dev`
    * macOS: `brew install boost`
* openssl >= 3.0.0 (Linux, macOS)
    * Linux: [install openssl 3](https://linuxhint.com/install-openssl-3-from-source/)
    * macOS: `brew install openssl@3`
    

## Basic Build Instructions

0. Clone this repo.
1. Edit the `CMakeLists.txt` -> `set(OPENSSL_ROOT_DIR "<path_to_openssl-3_installed_directory>")` regarding you Operating System.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake cmake  -DCMAKE_BUILD_TYPE=debug -DOPENSSL_ROOT_DIR=<root_directory_of_openssl> && make`
    * Example: `-DOPENSSL_ROOT_DIR=/usr/local/opt/openssl`
    * Note: You might need to take one further action:
        `export PKG_CONFIG_PATH="<path_to_openssl-3_pkgconfig>"`.
        * Example: `export PKG_CONFIG_PATH="/usr/local/opt/openssl@3/lib/pkgconfig"`
4. Run it: `./UniverWebSocket`.

** Please note that the examples in this repo, are for macOS.

or Simply:

0. You can only use below commands:
* `make debug` for Debug.
* `make build` to build the project.
* `make clean` to remove the build directory.
* `make style` to specify the format you want your code be like.
  * You will need to install `clang-format` first in case it is not installed. You only need to use it once.
  * In macOS you need to have `Homebrew` installed. The instructions on how to install, can be found [here](https://brew.sh). Then you can `brew install clang-format`.
  * In Ubuntu or Debian you can `sudo apt install clang-format`.

* `make format` to use the settings of the `style` which in this project is set by default to `-style=google`.
* `make all` to perform almost all the steps above at once.
* Run it: `./build/UniverWebSocket`.

## Next Step
* Binance combined stream can be accessed `/stream?streams=<streamName1>/<streamName2>/<streamName3>`. In this project, however, only the first stream can be accessed. Recommendations on solving this issue are welcomed.
* Log the connection information to a text file. 
