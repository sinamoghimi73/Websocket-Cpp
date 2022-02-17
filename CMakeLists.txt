cmake_minimum_required(VERSION 3.7)


set (PROJECT_NAME "UniverWebSocket")
project(${PROJECT_NAME})


set (PROJECT_SOURCE_DECLARATION_DIRECTORY ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DEFINITION_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
set (MAIN_FILE ${PROJECT_SOURCE_DEFINITION_DIRECTORY}/main.cpp)
include_directories (${PROJECT_SOURCE_DECLARATION_DIRECTORY})

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17 -pthread")
set(SYSTEM_OPENSSL=ON)

message(WARNING "Starting CMAKE...")
find_package(PkgConfig REQUIRED)
find_package(Boost COMPONENTS system thread regex REQUIRED)
message(WARNING "Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
message(WARNING "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(WARNING "Boost_VERSION: ${Boost_VERSION}")
include_directories(${Boost_INCLUDE_DIR})
# target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@3/")
find_package(openssl 3.0.1 REQUIRED)
message(WARNING "OpenSSL_VERSION: ${OPENSSL_VERSION}")
message(WARNING "OpenSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
message(WARNING "OpenSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")
include_directories(${OPENSSL_INCLUDE_DIR})



# Make relative paths absolute (needed later on)
foreach (p INCLUDE CMAKE)
  set (var INSTALL_${p}_DIR)
  if (NOT IS_ABSOLUTE "${${var}}")
    set (${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif ()
endforeach ()

# Find all executables
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
file(GLOB SOURCE_FILES src/main.cpp)

# Add project executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# "Link a target to given libraries."
target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
