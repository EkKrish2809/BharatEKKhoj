#version 460 core

in vec2 a_texcoord_out;

uniform vec2 res;
uniform sampler2D texture;

void main() {
    vec2 pixel = gl_FragCoord.xy / res.xy;
    gl_FragColor = texture2D( tex, pixel );//This is the color of the current pixel 
    gl_FragColor.r += 0.01;//Increment the red component 
 }