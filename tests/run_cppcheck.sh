cppcheck  --xml  \
          --verbose \
          --quiet \
          -i libs_required/OpenMesh/src/OpenMesh/Apps/Unsupported \
          -i build-release \
          -i build-debug \
          -i .svn \
          --force \
          -UDOXY_IGNORE_THIS \
          --suppress=unusedPrivateFunction \
          --suppress=missingInclude  \
          --suppress=ConfigurationNotChecked \
          --suppress=negativeIndex:Plugin-PoissonReconstruction/PoissonReconstruction/BSplineData.h:39 \
          --enable=style \
          --enable=performance \
          --enable=portability \
          --enable=information \
          --enable=missingInclude -j4 \
          . 2> cppcheck-result.xml


#--enable=all
