BASEPATH="../../../"


rm -f max/*
rm -f dbg/*

for i in $(ls -d $BASEPATH/Plugin*); do
  ln -s "../$(find $i/Deb* -name *Plugin* | grep lib | grep max | grep -v 64 )" max 2>&1 | grep -v "File exists"
  ln -s "../$(find $i/Deb* -name Plugin-*  | grep max | grep -v 64 )" max           2>&1 | grep -v "File exists"
  ln -s "../$(find $i/Deb* -name *Plugin* | grep lib | grep dbg | grep -v 64 )" dbg 2>&1 | grep -v "File exists"
  ln -s "../$(find $i/Deb* -name Plugin-*  | grep dbg | grep -v 64 )" dbg           2>&1 | grep -v "File exists"
done


