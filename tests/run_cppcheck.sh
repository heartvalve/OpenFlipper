cppcheck  --xml  \
          --verbose \
          --quiet \
          -i libs_required/OpenMesh/src/OpenMesh/Apps/Unsupported \
          -i build-release \
          --force \
          -UDOXY_IGNORE_THIS \
          --suppress=unusedPrivateFunction \
          --suppress=missingInclude  \
          --suppress=ConfigurationNotChecked \
          --suppress=negativeIndex:Plugin-PoissonReconstruction/PoissonReconstruction/BSplineData.h:39 \
          --enable=all . 2> cppcheck-result.xml

