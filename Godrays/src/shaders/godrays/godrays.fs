#version 460 core

in vec2 texcoord_out;
// in bool isLeft;

uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform float u_illuminationDecay;
uniform vec2 lightPositionOnScreen[2];

uniform sampler2D UserMapSampler;
uniform sampler2D ColorMapSampler;

const int NUM_SAMPLES = 500 ;

out vec4 FragColor;

void main()
{	
	float decay_s=0.97815;
	float exposure_s=0.2;
	float density_s=1.5926;
	float weight_s=0.58767;

	vec4 color;
	vec4 realColor = texture(UserMapSampler, texcoord_out);
	color = texture(ColorMapSampler, texcoord_out)*0.4;
	for (int clpos=0; clpos < 1; clpos++)
	{

		vec2 textCoo = texcoord_out;
		vec2 deltaTextCoord = vec2( textCoo - lightPositionOnScreen[clpos] );
		deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density_s;
		// float illuminationDecay = u_illuminationDecay;
		float illuminationDecay = 5.0;

		// vec4 sampleColor = texture2D(texture, texcoord_out );

		// color = texture2D(ColorMapSampler, texcoord_out)*0.4;

		for(int i=0; i < NUM_SAMPLES ; i++)
		{
				textCoo -= deltaTextCoord;
				vec4 sampleColor = texture(ColorMapSampler, textCoo )*0.4;
				sampleColor *= illuminationDecay * weight_s;
				color += sampleColor;
				illuminationDecay *= decay_s;
		}

	}

	FragColor = ((vec4((vec3(color.r, color.g, color.b) * exposure_s), 1.0)) + (realColor * (1.1)));
	
}
