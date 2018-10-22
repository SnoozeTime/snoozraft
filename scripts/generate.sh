#!/bin/bash

DESTINATION="../src/protocol/"
GENERATED_FOLDER="generated"

rm -r ${GENERATED_FOLDER}
mkdir -p ${GENERATED_FOLDER}/messages

# First, generate the files
pipenv run python generate.py messages.yaml

# Run clang-format on the folder
for filename in ${GENERATED_FOLDER}/*.h; do
    [ -e "$filename" ] || continue
    clang-format -i ${filename}
done

for filename in ${GENERATED_FOLDER}/messages/*.h; do
    [ -e "$filename" ] || continue
    clang-format -i ${filename}
done

# Move them !
rm -r ${DESTINATION}/messages
mv ${GENERATED_FOLDER}/* $DESTINATION
