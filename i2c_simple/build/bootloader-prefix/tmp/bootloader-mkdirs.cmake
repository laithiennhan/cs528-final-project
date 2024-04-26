# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/hieunguyen/esp/esp-idf/components/bootloader/subproject"
  "/Users/hieunguyen/Desktop/cs528-final-project/i2c_simple/build/bootloader"
  "/Users/hieunguyen/Desktop/cs528-final-project/i2c_simple/build/bootloader-prefix"
  "/Users/hieunguyen/Desktop/cs528-final-project/i2c_simple/build/bootloader-prefix/tmp"
  "/Users/hieunguyen/Desktop/cs528-final-project/i2c_simple/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/hieunguyen/Desktop/cs528-final-project/i2c_simple/build/bootloader-prefix/src"
  "/Users/hieunguyen/Desktop/cs528-final-project/i2c_simple/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/hieunguyen/Desktop/cs528-final-project/i2c_simple/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/hieunguyen/Desktop/cs528-final-project/i2c_simple/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
