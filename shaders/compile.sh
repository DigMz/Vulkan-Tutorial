project_path="/home/digmz/Documents/C++/Vulkan/Vulkan-Tutorial/"
shader_path="${project_path%/}/shaders"

shader_vert_path="${shader_path%/}/shader.vert"
shader_frag_path="${shader_path%/}/shader.frag"
shader_vert_out="${shader_path%/}/vert.spv"
shader_frag_out="${shader_path%/}/frag.spv"

glslc "$shader_vert_path" -o "$shader_vert_out"
glslc "$shader_frag_path" -o "$shader_frag_out"
