#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in mat3 TBN;

layout(set = 1, binding = 0) uniform MaterialUBO {
    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;
    vec4 emissiveColor;
    float shininess;
    float opacity;
    float refractiveIndex;
} material;

layout(set = 1, binding = 1) uniform sampler2D diffuseMap;
layout(set = 1, binding = 2) uniform sampler2D specularMap;
layout(set = 1, binding = 3) uniform sampler2D normalMap;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 viewDir = normalize(vec3(1.0, 1.0, -1.0) - fragPos);

    vec4 diffuseTexColor = texture(diffuseMap, fragTexCoord);
    vec4 specularTexColor = texture(specularMap, fragTexCoord);

    vec3 normalMapColor = texture(normalMap, fragTexCoord).rgb;
    vec3 normal = normalize(TBN * (normalMapColor * 2.0 - 1.0));

    vec3 ambient = material.ambientColor.rgb;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuseColor.rgb * diffuseTexColor.rgb;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specularColor.rgb * specularTexColor.rgb;

    vec3 result = ambient + diffuse + specular + material.emissiveColor.rgb;

    float alpha = diffuseTexColor.a * material.opacity;
    outColor = vec4(result, alpha);
}