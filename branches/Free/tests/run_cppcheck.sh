cppcheck  --xml  \
          --verbose \
          --quiet \
          -i libs_required/OpenMesh/src/OpenMesh \
          -i libs_required/OpenMesh/src/OpenVolumeMesh \
          -i Plugin-MemInfo/procps \
          -i Plugin-PoissonReconstruction \
          -i build-release \
          -i build-debug \
          -i .svn \
          --force \
          -UDOXY_IGNORE_THIS \
          --suppress=unusedPrivateFunction \
          --suppress=ConfigurationNotChecked \
          --suppressions-list=tests/cppcheck-suppressions.txt \
          --enable=style \
          --enable=performance \
          --enable=portability \
          --enable=information \
          --enable=missingInclude -j2 \
          . 2> cppcheck-result.xml


#--enable=all
