#!/bin/bash
WD=`pwd`
OUTPUT_PATH=$1
EXECFILE=$2
DRIVER=$3
shift 3

ENTRIES="[\n"

ENTRIES=${ENTRIES}"  {\n   \"directory\" : \"$WD\",\n"
ENTRIES=${ENTRIES}"    \"command\" : \"clang++ -std=c++17 $DRIVER -o $EXECFILE\",\n"
ENTRIES=${ENTRIES}"    \"file\" : \"$WD/$DRIVER\"\n  },\n"

for implem in "$@"
do
  ENTRIES=${ENTRIES}"  {\n   \"directory\" : \"$WD\",\n"
  ENTRIES=${ENTRIES}"    \"command\" : \"clang++ -std=c++17 $implem -o $EXECFILE\",\n"
  ENTRIES=${ENTRIES}"    \"file\" : \"$WD/$implem\"\n  },\n"
done

ENTRIES=${ENTRIES}"]"

echo -e $ENTRIES > $OUTPUT_PATH/compile_commands.json
