#version 460 core

in float Height;
in vec2 a_texcoord_tes_out;

uniform sampler2D terrainTex;

out vec4 FragColor;

void main()
{
    float h = (Height + 16)/64.0f;
    FragColor = vec4(h, h, h, 1.0) + texture(terrainTex, a_texcoord_tes_out);
    // FragColor = vec4(1.0, 0.5, 0.7, 1.0);
}
