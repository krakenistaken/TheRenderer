#version 330 core
in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoord;

uniform vec3 uViewPos;
uniform sampler2D uTexture;
uniform int uHasTexture;

out vec4 FragColor;

void main() {
    // Material
    vec3 objectColor = vec3(0.35, 0.55, 0.78);
    if (uHasTexture == 1) {
        objectColor = texture(uTexture, vTexCoord).rgb;
    }
    vec3 ambientColor = vec3(0.08, 0.08, 0.12);

    // Directional light
    vec3 lightDir = normalize(vec3(0.8, 1.0, 0.6));
    vec3 lightColor = vec3(1.0, 0.97, 0.92);

    // Secondary fill light (dimmer, from opposite side)
    vec3 fillDir = normalize(vec3(-0.5, 0.3, -0.8));
    vec3 fillColor = vec3(0.15, 0.18, 0.25);

    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(uViewPos - vFragPos);

    // --- Main light ---
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * 0.7;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor * 0.6;

    // --- Fill light (diffuse only) ---
    float fillDiff = max(dot(normal, fillDir), 0.0);
    vec3 fill = fillDiff * fillColor;

    // Rim lighting for extra visual pop
    float rim = 1.0 - max(dot(viewDir, normal), 0.0);
    rim = smoothstep(0.55, 1.0, rim);
    vec3 rimColor = rim * vec3(0.2, 0.3, 0.5) * 0.4;

    vec3 result = (ambientColor + diffuse + specular + fill + rimColor) * objectColor;

    // Tone mapping (simple Reinhard)
    result = result / (result + vec3(1.0));

    FragColor = vec4(result, 1.0);
}
