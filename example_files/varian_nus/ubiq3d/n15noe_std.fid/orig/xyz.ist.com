#!/bin/csh

basicFT3.com -in data/test%03d.fid -out ist/test%03d.ft3 \
 -xP0 -201 -xP1 0 -xEXTX1 10.0ppm -xEXTXN 5.5ppm \
 -yP0   45 -yP1 0 -zP0 0 -zP1 0 -nusZF -ist

proj3D.tcl -in ist/test%03d.ft3

