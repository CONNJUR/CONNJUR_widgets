#!/bin/csh

foreach j (*)
   if (-e $j/clean.com) then
      echo $j
      cd $j
      clean.com
      cd ../
   endif
end
