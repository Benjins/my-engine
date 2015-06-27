#!/bin/bash


echo $CXX

#g++4.8.1
if [ "$CXX" == "g++" ]; then sudo apt-get install -qq gcc-4.8 g++-4.8; fi
if [ "$CXX" == "g++" ]; then sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90; fi

# clang 3.4
if [ "$CXX" == "clang++" ]; then sudo apt-get install --allow-unauthenticated -qq clang-3.4; fi
if [ "$CXX" == "clang++" ]; then export CXX="clang++-3.4"; fi