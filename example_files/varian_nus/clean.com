#!/bin/csh

foreach i (*)
   if (-e $i/clean.com) then
      echo $i
      cd $i
      clean.com
      cd ../
   endif
end
