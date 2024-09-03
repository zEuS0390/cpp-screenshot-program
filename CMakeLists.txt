# Set the minimum required version of CMake for this project
cmake_minimum_required(VERSION 3.21)

# Define the project name (cpp-screenshot-program)
project(cpp-screenshot-program)

# Create an executable for the project. The "WIN32" keyword indicates that this
# is a Windows GUI application, which means it will not create a console window
# when the program is launched.
add_executable(cpp-screenshot-program WIN32 main.cpp)

