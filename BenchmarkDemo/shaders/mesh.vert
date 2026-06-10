#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 aInstanceMatrix;

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vFragPos;
out vec3 vNormal;
out vec2 vTexCoord;

void main() {
    vec4 worldPos = aInstanceMatrix * vec4(aPosition, 1.0);
    vFragPos = worldPos.xyz;
    // For uniform scale, mat3(aInstanceMatrix) suffices as normal matrix
    vNormal = mat3(aInstanceMatrix) * aNormal;
    vTexCoord = aTexCoord;
    gl_Position = uProjection * uView * worldPos;
}
