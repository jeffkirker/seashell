# seashell

This is a basic shell written in C++. This shell demonstrates the concepts of system multi-processing, and includes an implementation of a basic process table.

## Usage

Compilation is automated. Clone the repository and type `make` into the terminal. Some warnings may be given, they can safely be ignored. Some systems may give errors, which unfortunately will prevent the shell from working properly. This shell has been tested on macOS and Ubuntu only.

## Commands 

Most standard shell commands will work, with some exceptions. In addition, the following built-in commands have been implemented:

`jobs` - view the process table displaying details about any currently running processes

`kill [pid]` - kill process with indicated process

`suspend [pid]` - stop the process with indicated process 

`resume [pid]` - resume execution of indicated process

`sleep [seconds]` - create a sleep process for the indicated length of time.

`exit` - exit the shell. 

Note: this shell creates real system processes, which will continue to run if `ctrl + c` is used. Using the built-in `exit` command ensures that all processes are ended.

## Disclaimer

Do not use this to cheat on your homework. Your professor is smarter than you, and he will find this source if you copy it. The contents of your assignments are infinitely more valuable to you if you take the time to learn them yourself. 
