#!/bin/sh

cp -r OpenFlipper/Icons OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents
cp -r OpenFlipper/Shaders OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents
cp -r OpenFlipper/Textures OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents
cp -r OpenFlipper/Fonts OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents

if [ -d /usr/local/Trolltech/Qt-4.4.3/plugins ]; then
  cp -r /usr/local/Trolltech/Qt-4.4.3/plugins OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents/qtplugins
else
  echo "Unable to find qt Plugin directory"
fi

#Create Frameworks directory
if [ ! -d OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents/Frameworks ]; then
   mkdir OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents/Frameworks
   echo Created OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents/Frameworks
fi

cp -r Arch/macx/Resources OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents 

# Copy All basic libs to Frameworks 
for i in $(find . -name *.dylib | grep -v "Plugin-" | grep Release ); do
   cp $i OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents/Frameworks
done;

pushd .

cd OpenFlipper/Darwin_32_Release/OpenFlipper.app/Contents/MacOS

# extract Qt dependencies and place them in Frameworks directory
# and change the executables path to the right dir
for i in $(otool -L OpenFlipper | awk -F " " '{print $1}' | grep -v System | grep -v Developer | grep Qt) ; do
        NEW=$(echo $i | awk -F "/" '{print $NF}')
        OLD=$(echo $i)

        cp $OLD ../Frameworks

        install_name_tool -change $OLD @executable_path/../Frameworks/$NEW  OpenFlipper

done

# extract opt/local dependencies and place them in Frameworks directory
# and change the executables path to the right dir
for i in $(otool -L OpenFlipper | awk -F " " '{print $1}' | grep -v System | grep -v Developer | grep "/opt/local" ) ; do
        NEW=$(echo $i | awk -F "/" '{print $NF}')
        OLD=$(echo $i)
        cp $OLD ../Frameworks
        install_name_tool -change $OLD @executable_path/../Frameworks/$NEW  OpenFlipper

done

# Change OpenFlippers own dependencies to OpenMesh ACG and Pluginlib
# and change the executables path to the right dir
for i in $(otool -L OpenFlipper | awk -F " " '{print $1}' | grep OpenFlipper ) ; do
        NEW=$(echo $i | awk -F "/" '{print $NF}')
        OLD=$(echo $i)
        install_name_tool -change $OLD @executable_path/../Frameworks/$NEW  OpenFlipper
done

cd ..

cd qtplugins

 # Change paths in the qtplugins
 for DIR in $(ls -1 ); do
   if [ -d $DIR ]; then
     cd $DIR

     for file in $(ls -1 ); do
       for i in $(otool -L $file | awk -F " " '{print $1}' | grep Qt ) ; do
         NEW=$(echo $i | awk -F "/" '{print $NF}')
         OLD=$(echo $i)
         install_name_tool -change $OLD @executable_path/../Frameworks/$NEW $file
       done
     done

     cd ..

   fi

 done

cd ..

pushd .

cd Plugins
cd Darwin 
cd 32
cd Release

# extract opt/local dependencies for Plugins and place them in Frameworks directory up to level 5
for file in $(ls -1 ); do
  if [ ! -d $file ]; then

	  for i in $(otool -L $file | awk -F " " '{print $1}' | grep -v System | grep -v Developer | grep -v "/usr/lib" | grep -v "libPlugin-") ; do
        	NEW=$(echo $i | awk -F "/" '{print $NF}')
	        OLD=$(echo $i)
		if [ ! -e ../../../../Frameworks/$NEW ]; then
			if [ -e $OLD ]; then
				cp -n $OLD ../../../../Frameworks
			fi
		fi
		
	  done
  fi
done

popd

cd Frameworks

#Check files in framework dir for dependencies and copy them to the Frameworks directory
for file in $(ls -1 ); do
  for i in $(otool -L $file | awk -F " " '{print $1}' | grep -v System | grep -v Developer | grep -v "/usr/lib" ) ; do
       	NEW=$(echo $i | awk -F "/" '{print $NF}')
        OLD=$(echo $i)

	if [ ! -e ../Frameworks/$NEW ]; then
		if [ -e $OLD ]; then
			cp -n $OLD ../Frameworks
		fi
	fi                                  	

  done
done

# Change paths in the framework files
for file in $(ls -1 ); do
  for i in $(otool -L $file | awk -F " " '{print $1}' | grep -v System | grep -v Developer | grep -v "/usr/lib") ; do
        NEW=$(echo $i | awk -F "/" '{print $NF}')
        OLD=$(echo $i)
        install_name_tool -change $OLD @executable_path/../Frameworks/$NEW $file
  done
done

# Change ids in the qt framework files
for file in $(ls -1 ); do 
 COUNT=1;

 for i in $(otool -L $file | awk -F " " '{print $1}' | grep -v System | grep -v Developer | grep -v "/usr/lib" ) ; do
        
        NEW=$(echo $i | awk -F "/" '{print $NF}')

        if [ $COUNT == 2 ]; then
          install_name_tool -id @executable_path/../Frameworks/$NEW $file
        fi

        COUNT=$((COUNT+1))
  done

done


cd ..

pushd .

cd Plugins
cd Darwin 
cd 32
cd Release


 # Change paths in the plugins
 for file in $(ls -1 ); do
   if [ ! -d $file ]; then
     for i in $(otool -L $file | awk -F " " '{print $1}' | grep -v System | grep -v Developer | grep -v "/usr/lib") ; do
          NEW=$(echo $i | awk -F "/" '{print $NF}')
          OLD=$(echo $i)
          install_name_tool -change $OLD @executable_path/../Frameworks/$NEW $file
     done
   fi
 done

popd 

find . -name *.svn | xargs rm -rf 

popd


