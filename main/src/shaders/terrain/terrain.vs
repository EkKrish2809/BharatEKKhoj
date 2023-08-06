#version 460 core
in vec3 a_position;
in vec2 a_texcoord;
// in vec3 a_normal;
// in vec3 a_tangent;
// in vec3 a_bittangent;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

// uniform vec3 lightPos;
// uniform vec3 viewPos;

uniform vec4 u_plane;

out vec2 a_texCoord_out;
// out vec3 FragPos;
// out vec3 TangentLightPos;
// out vec3 TangentViewPos;
// out vec3 TangentFragPos;

void main()
{
    a_texCoord_out = a_texcoord;

    // normal mapping changes
    // FragPos = vec3(u_modelMatrix * vec4(a_position, 1.0));   
    
    // mat3 normalMatrix = transpose(inverse(mat3(u_modelMatrix)));
    // vec3 T = normalize(normalMatrix * a_tangent);
    // vec3 N = normalize(normalMatrix * a_normal);
    // T = normalize(T - dot(T, N) * N);
    // vec3 B = cross(N, T);
    
    // mat3 TBN = transpose(mat3(T, B, N));    
    // TangentLightPos = TBN * lightPos;
    // TangentViewPos  = TBN * viewPos;
    // TangentFragPos  = TBN * FragPos;

    //gl_ClipDistance[0] = dot(u_modelMatrix * a_position, u_plane);

    // gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0);
    gl_Position = vec4(a_position, 1.0);
    
}
