#version 450

// input colors
layout(location = 0) in vec3 fragColor;

// returning colors
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}

