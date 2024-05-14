#!/bin/sh

# This script automates the build and execution process for a project.

# Stop the script if any command fails
set -e

# Step 1: Compile the project using make
echo "Compiling the project..."
make -j4

# Step 2: Render a scene using PBRT with the provided scene name
echo "Rendering the scene with PBRT..."
./pbrt ../scenes/"$1".pbrt

# Step 3: Convert an EXR image to PNG format
echo "Converting EXR to PNG..."
./imgtool convert "$1".exr ./"$1".png

# Step 4: Optionally open the PNG image, based on the second argument
if [ "$2" != "false" ]; then
    echo "Opening the image..."
    open "$1".png
else
    echo "Not opening the image as requested."
fi

echo "Script completed successfully."

