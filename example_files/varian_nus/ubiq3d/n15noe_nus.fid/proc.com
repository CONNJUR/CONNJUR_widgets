#!/bin/csh

basicFT3.com -in data/test%03d.fid -out ft/test%03d.ft3 \
 -xP0 -201 -xP1 0 -xEXTX1 10.0ppm -xEXTXN 5.5ppm   \
 -yP0   45 -yP1 0 -zP0 0 -zP1 0 

proj3D.tcl -in ft/test%03d.ft3 

ist3D.com $* -istMaxRes Auto \
 -in data/test%03d.fid -mask mask/test%03d.fid -out ist/test%03d.ft3 \
 -xP0 -201 -xP1 0 -xEXTX1 10.0ppm -xEXTXN 5.5ppm   \
 -yP0   45 -yP1 0 -zP0 0 -zP1 0 


