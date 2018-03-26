start-app
=========

An empty cross-platform `ddui` app that can be used to quickly get started with app-making.

## Download

Clone the repository, and then re-initialise the repository:

```
$ git clone --recursive https://github.com/bartjoyce/start-app your-app
$ cd your-app
$ rm -rf .git/
$ git init
$ git submodule add https://github.com/bartjoyce/ddui lib/ddui
$ git submodule update --init --recursive
$ git add -A .
$ git commit -m 'First commit'
```

## Build

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
