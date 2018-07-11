#!/bin/csh

#
# Processing with NUS Zero Fill (extrapolation of time-domain):
#  Turn on NUS Zero Fill using the -nusZF flag.
#  Select IST as the reconstruction method with the -ist flag.
#  Results are rescaled to max 100.0 for easier comparison.

basicFT2.com -in test.fid -out ist.ft2 \
             -xP0 25.42 -xP1 0 -yP0 0 -yP1 0 -scaleTo 100.0 \
             -nusZF -ist -title US_NUSZF_IST 


