#!/bin/bash
# Get the absolute path of the script
SCRIPT=$(readlink -f "$0")
# Get the directory containing the script
SCRIPTPATH=$(dirname "$SCRIPT")
echo "Script directory: $SCRIPTPATH"

shader_vert_path="${SCRIPTPATH%/}/shader.vert"
shader_frag_path="${SCRIPTPATH%/}/shader.frag"
shader_vert_out="${SCRIPTPATH%/}/vert.spv"
shader_frag_out="${SCRIPTPATH%/}/frag.spv"

glslc "$shader_vert_path" -o "$shader_vert_out"
glslc "$shader_frag_path" -o "$shader_frag_out"
