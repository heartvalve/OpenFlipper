BASEPATH="../../../"


rm -f max/*
rm -f dbg/*

for i in $(ls -d $BASEPATH/Plugin*); do
  ln -s "../$(find $i/Deb* -name *Plugin* | grep lib | grep max | grep 64 )" max
  ln -s "../$(find $i/Deb* -name *Plugin* | grep lib | grep dbg | grep 64 )" dbg
done


