#!/bin/csh

varian $* -nosleep -user \
 -xCAR 4.7701 -yCAR 4.7701 -yOBS 599.422 -yLAB 1H -zCAR 118.1781 

