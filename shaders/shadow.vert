#version 450

layout(binding = 0) uniform LightSpaceMatrix {
    mat4 lightSpaceMatrix;
} ls;

layout(location = 0) in vec3 inPosition;

void main() {
    gl_Position = ls.lightSpaceMatrix * vec4(inPosition, 1.0);
}
