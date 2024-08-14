#!/bin/bash

# Run Doxygen
doxygen Doxyfile

# Change directory to docs
cd ./docs

# Run moxygen to convert xml doc to md
moxygen xml

# Change directory to docs
cd ..

# Remove xml documentation
rm -r xml

# C program to upload documentation to wiki
# Export API key
# Run Program

echo "Documentation updated and converted to markdown"
