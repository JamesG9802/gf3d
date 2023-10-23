#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 ambient;
layout(location = 4) in vec4 lightPosition;
layout(location = 5) in vec4 vertPosition;

layout(location = 0) out vec4 outColor;


void main()
{
    vec3 lightVector = lightPosition.xyz - vertPosition.xyz;
    float lightDistance = length(lightVector);
    lightVector = normalize(lightVector);
    float cosTheta = max(dot( normalize(fragNormal),lightVector ), 0);
    vec4 baseColor = texture(texSampler, fragTexCoord);

    cosTheta = 50;
    if(lightDistance > 1)
    {
        cosTheta = 50/lightDistance;
    } 
 

    outColor = (baseColor * ambient) + baseColor * cosTheta;
    outColor.x = outColor.x * colorMod.x;
    outColor.y = outColor.y * colorMod.y;
    outColor.z = outColor.z * colorMod.z;
    outColor.w = baseColor.w * colorMod.w;
}
