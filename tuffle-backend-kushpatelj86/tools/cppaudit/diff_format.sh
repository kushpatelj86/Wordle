#!/bin/bash
OS_NAME=`uname -s | tr A-Z a-z`

# Check if the first parameter is a flag starting with --. If so, that's
# a flag we want to pass to clang-format. Shift the other parameters
# and continue.
if [[ $1 =~ --.* ]]; then
  flag="$1"
  shift
fi


for file in "$@"
do
  echo -e "\n[Checking $file]"
  if [[ "$OS_NAME" != "darwin" ]]; then
    diff --suppress-common-lines --color=always -u1 <(cat $file) <(clang-format $flag $file)
  else
    diff --suppress-common-lines -U1 <(cat $file) <(clang-format $flag $file)
  fi
done
exit 0
