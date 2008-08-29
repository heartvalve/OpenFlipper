BASEPATH="../../../"


rm -f max/*
rm -f dbg/*

for i in $(ls -d $BASEPATH/Plugin*); do
  ln -s "../$(find $i/Deb* -name *Plugin* | grep lib | grep max | grep -v 64 )" max
  ln -s "../$(find $i/Deb* -name *Plugin* | grep lib | grep dbg | grep -v 64 )" dbg
done


