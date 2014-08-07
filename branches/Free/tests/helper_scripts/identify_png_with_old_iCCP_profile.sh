#!/bin/sh
for i in $(find . -name "*.png"); do
  if ( identify -verbose $i | grep -q iCCP ) then
    echo "BAD: $i"
    #convert $i -strip $i
  fi
done
