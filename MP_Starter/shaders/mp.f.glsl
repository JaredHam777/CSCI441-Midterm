#version 410 core

// uniform inputs

// varying inputs
layout(location = 0) in vec3 color;     // interpolated color for this fragment

// outputs
out vec4 fragColorOut;                  // color to apply to this fragment

void main() {
    // pass the interpolated color through as output
    fragColorOut = vec4(color, 1.0);
}