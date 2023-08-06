#version 460 core

in vec3 a_position_out;
in float a_lightIntensity_out;

uniform vec3 u_offset;
uniform vec3 u_skyColor;
uniform vec3 u_cloudColor;
uniform sampler3D u_noise;

out vec4 FragColor;
void main(void)
{
	vec4 noisevec = texture(u_noise, 1.2 * (vec3 (0.5) + a_position_out + u_offset));
    vec3 color;
    float intensity = (noisevec[0] + noisevec[1] + noisevec[2] + noisevec[3]) * 1.7;

    // intensity = 1.95 * abs(2.0 * intensity - 1.0);
    intensity = 1.55 * abs(2.0 * intensity - 1.0);
    intensity = clamp(intensity, 0.0, 1.0);

	// color = mix(u_cloudColor, u_skyColor, intensity) * a_lightIntensity_out;
	color = mix(u_cloudColor, u_skyColor, intensity) ;
    color = clamp(color, 0.0, 1.0);

    // if (color.r < 0.9)
    //     discard;
    // if (color.g < 0.6)
    //     discard;
    // if (color.b < 0.8)
    //     discard;

	FragColor = vec4(color, 1.0);


}
