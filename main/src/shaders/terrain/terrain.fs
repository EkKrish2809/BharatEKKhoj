#version 460 core

in float Height;
in vec2 a_texcoord_tes_out;

// in vec3 FragPos;
// in vec3 TangentLightPos;
// in vec3 TangentViewPos;
// in vec3 TangentFragPos;

uniform sampler2D terrainTex;
// uniform sampler2D normalMap;

// uniform vec3 lightPos;
// uniform vec3 viewPos;

out vec4 FragColor;

// vec4 calculateNormal()
// {
    // vec4 normalColor = vec4(0.0);
    // // obtain normal from normal map in range [0,1]
    // vec3 normal = texture(normalMap, a_texcoord_tes_out).rgb;
    // // transform normal vector to range [-1,1]
    // normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // // get diffuse color
    // vec3 color = texture(terrainTex, a_texcoord_tes_out).rgb;
    // // ambient
    // vec3 ambient = 0.1 * color;
    // // diffuse
    // vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    // float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * color;
    // // specular
    // vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    // vec3 halfwayDir = normalize(lightDir + viewDir);  
    // float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    // vec3 specular = vec3(0.2) * spec;
    // return normalColor = vec4(ambient + diffuse + specular, 1.0);
// }

void main()
{
    float h = (Height + 16)/64.0f;
    FragColor = vec4(h, h, h, 1.0) + texture(terrainTex, a_texcoord_tes_out);// + calculateNormal();
    // FragColor = vec4(1.0, 0.5, 0.7, 1.0);

    // for HDR bloom
    // float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    // if(brightness > 0.5)
    //     FragColor = vec4(FragColor.rgb, 1.0);
    // else
    //     FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
