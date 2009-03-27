#!/bin/bash

# Absolute path to the script
SCRIPT=$(readlink -f $0);
# Absolute path of the directory
SCRIPTPATH=`dirname $SCRIPT`"/";

base=$SCRIPTPATH"../../";

ACG=$base"ACG/lib/";
OpenMeshCore=$base"OpenMesh/Core/lib/";
OpenMeshTools=$base"OpenMesh/Tools/lib/";
OpenFlipper=$base"OpenFlipper/";
PluginLib=$OpenFlipper"PluginLib/lib/";
Icons=$OpenFlipper"Icons/*.png";
Plugins=$OpenFlipper"Plugins/Linux/";

shareBase=$SCRIPTPATH"tmp/usr/share/OpenFlipper/";
shareIcons=$shareBase"Icons";
sharePlugins=$shareBase"Plugins/Linux/";


for Architecture in "32" "64"; do

  Arch="Debian_"${Architecture}"_Release/";

  #check if the architecture was build
  if [[ ! -e ${OpenFlipper}${Arch}"OpenFlipper"\
     || ! -e ${ACG}${Arch}"libACG.so.1"\
     || ! -e ${OpenMeshCore}${Arch}"libCore.so.1"\
     || ! -e ${OpenMeshTools}${Arch}"libTools.so.1"\
     || ! -e ${PluginLib}${Arch}"libPluginLib.so.1.0.0" ]]; then
    continue;
  fi

  echo "Found Architecture: "${Architecture}"-Bit";

  allArch=${allArch}" "${Architecture};

  #copy Libs/Binary
  mkdir -p ${shareBase}${Arch}
  cp -f ${OpenFlipper}${Arch}"OpenFlipper" ${shareBase}${Arch};
  cp -f ${ACG}${Arch}"libACG.so.1" ${shareBase}${Arch};
  cp -f ${OpenMeshCore}${Arch}"libCore.so.1" ${shareBase}${Arch};
  cp -f ${OpenMeshTools}${Arch}"libTools.so.1" ${shareBase}${Arch};
  cp -f ${PluginLib}${Arch}"libPluginLib.so.1" ${shareBase}${Arch};

  #copy Plugins
  mkdir -p $sharePlugins$Architecture"/Release/";
  cp -f ${Plugins}${Architecture}/Release/*.so $sharePlugins$Architecture"/Release/";

done

#check which architectures we have copied
if [[ $allArch == *32* && $allArch == *64* ]]; then
  Architecture="all";
elif [[ $allArch == *32* ]]; then
  Architecture="i386";
elif [[ $allArch == *64* ]]; then
  Architecture="amd64";
else
  #abort
  rm -rf tmp;
  exit;
fi

#copy Icons
mkdir -p $shareIcons;
cp -f $Icons $shareIcons;

#copy config files
cp -rf $SCRIPTPATH"DEBIAN" $SCRIPTPATH"/tmp/DEBIAN";
mkdir -p $SCRIPTPATH"/tmp/usr/share/applications/";
cp -f $SCRIPTPATH"OpenFlipper.desktop" $SCRIPTPATH"/tmp/usr/share/applications/";

#modify architecture/size in config file
sed -i 's/Architecture: all/Architecture: '${Architecture}'/g' $SCRIPTPATH"/tmp/DEBIAN/control"
Size=`du -s tmp|tr -d ' tmp/'`;
echo "Project Size: "$Size;
sed -i "s/Installed-Size:/Installed-Size: $Size/g" $SCRIPTPATH"/tmp/DEBIAN/control"

#build the package
dpkg -b tmp OpenFlipper-1.0_${Architecture}.deb

#remove temp files
rm -rf tmp;
