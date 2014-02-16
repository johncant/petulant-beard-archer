#!/bin/bash
#
# TODO - Too lazy to use autotools atm

g++ src/*.cc -o photogrammetry `pkg-config --libs --cflags gtkmm-3.0 gl` -lX11 -g
