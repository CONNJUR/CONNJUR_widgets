#!/bin/csh

#
# Processing of conventional Uniformly Sampled (US) 2D data.
# Prepare a version of the FID with the indirect dim truncated to 25%:

nmrPipe -in test.fid \
| nmrPipe -fn TP \
| nmrPipe -fn EXT -xn 25% -sw \
| nmrPipe -fn TP \
   -out trunc.fid -ov

#
# Process the full and truncated versions
#  Zero filling is set so that results will have the same size.
#  Results are rescaled to max 100.0 for easier comparison.

basicFT2.com -in test.fid  -out test.ft2  \
 -xP0 25.42 -xP1 0 -yP0 0 -yP1 0 -yZFARG zf=2,auto \
 -scaleTo 100.0 -title US_FULL_FT 

basicFT2.com -in trunc.fid -out trunc.ft2 \
 -xP0 25.42 -xP1 0 -yP0 0 -yP1 0 -yZFARG zf=4,auto \
 -scaleTo 100.0 -title US_25pct_FT


