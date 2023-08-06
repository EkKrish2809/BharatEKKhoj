#version 460 core

precision highp float;
precision highp sampler2D;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 a_texcoord_out;

uniform sampler2D u_hdrTextureSampler;

void main()
{             
    FragColor = texture(u_hdrTextureSampler, a_texcoord_out);      
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 0.1)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
