#version 460 core

in vec2 a_texcoord_out;

uniform sampler2D u_textureSampler;
uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

out vec4 FragColor;

void main(void)
{
    vec2 tex_offset = 1.0 / textureSize(u_textureSampler, 0);				  // gets size of single texel
    vec3 result = texture(u_textureSampler, a_texcoord_out).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(u_textureSampler, a_texcoord_out + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(u_textureSampler, a_texcoord_out - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(u_textureSampler, a_texcoord_out + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(u_textureSampler, a_texcoord_out - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
