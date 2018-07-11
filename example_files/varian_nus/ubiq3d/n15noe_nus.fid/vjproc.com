#!/bin/csh

#
# Example processing using VnmrJ 4.2 NMRPipe script generation:

vj2pipe.com $* -xEXTX1 10.0ppm -xEXTXN 5.5ppm -xCAR 4.7701 -zCAR 118.1781 -istMaxRes Auto 
