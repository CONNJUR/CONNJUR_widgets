#!/bin/csh


basicFT2.com -xy -in data/test%03d.fid -out xy.ft2 \
 -xP0 -201 -xP1 0 -xEXTX1 10.0ppm -xEXTXN 5.5ppm   \
 -yP0   45 -yP1 0 

basicFT2.com -xz -in data/test%03d.fid -out xz.ft2 -zPlane 1 \
 -xP0 -201 -xP1 0 -xEXTX1 10.0ppm -xEXTXN 5.5ppm   \
 -yP0    0 -yP1 0 

basicFT3.com -in data/test%03d.fid -out ft/test%03d.ft3 \
 -xP0 -201 -xP1 0 -xEXTX1 10.0ppm -xEXTXN 5.5ppm   \
 -yP0   45 -yP1 0 -zP0 0 -zP1 0 

