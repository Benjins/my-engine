#!/bin/bash

# g++4.8.1
if [ "$CXX" == "g++" ]; then sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test; fi

# clang 3.4
if [ "$CXX" == "clang++" ]; then sudo add-apt-repository -y ppa:h-rayflood/llvm; fi
sudo apt-get install freeglut3-dev glew1.5-dev libfreetype6-dev libopenal-dev sshpass doxygen
sudo apt-get update -qq
