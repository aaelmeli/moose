#!/bin/bash
# Regenerate all Flex+BISON generated files
# Assumes the file has a FlexBison*.sh script adjacent
# Run this from the root directory
for i in `find ./ -name 'FlexBison*.sh'`; do pushd `dirname $i` && ./`basename $i` && popd; done
