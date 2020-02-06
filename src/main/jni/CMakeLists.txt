cmake_minimum_required(VERSION 2.8)

project(jni)

#TODO: auto include dirs
include_directories("./include" "/usr/lib/jvm/java-8-openjdk-amd64/include" "/usr/lib/jvm/java-8-openjdk-amd64/include/linux")

subdirs(cpp)