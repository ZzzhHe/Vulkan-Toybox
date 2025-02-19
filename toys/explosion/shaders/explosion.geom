#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec3 fragNormalIn[];
layout (location = 1) in vec2 fragTexCoordIn[];

layout (location = 0) out vec3 fragNormalOut;
layout (location = 1) out vec2 fragTexCoordOut;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
} ubo;

void main() {
    vec4 center = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
    
    vec3 direction = normalize(center.xyz);

    float explosionPower = min(ubo.time * 2, 2.0);
    
    for (int i = 0; i < 3; i ++) {
        vec4 position = gl_in[i].gl_Position;
        position.xyz += direction * explosionPower;

        float angle = ubo.time * 3.14159 * 2.0;

        gl_Position = position;
        fragNormalOut = fragNormalIn[i];
        fragTexCoordOut = fragTexCoordIn[i];
        EmitVertex();
    }
    EndPrimitive();

}
