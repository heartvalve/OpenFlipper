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
          --suppressions-list=tests/cppcheck-suppressions.txt \
          --enable=style \
          --enable=performance \
          --enable=portability \
          --enable=information \
          --enable=missingInclude -j2 \
          . 2> cppcheck-result.xml


#--enable=all
