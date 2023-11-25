#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 ambient;
layout(location = 4) in vec4 lightPosition;
layout(location = 5) in vec4 cameraPosition;
layout(location = 6) in vec4 vertPosition;

layout(location = 0) out vec4 outColor;

// based on stippling effect described by https://www.youtube.com/watch?v=--GB9qyZJqg 4:35
const mat4 thresholdMatrix = mat4(
    1, 9, 3, 11,
    13, 5, 15, 7,
    4, 12, 2, 10,
    16, 8, 14, 6
);

void main()
{

    vec3 lightVector = normalize(lightPosition.xyz - vertPosition.xyz);
    float cosTheta = max(dot( normalize(fragNormal),lightVector ), 0);
    vec4 baseColor = texture(texSampler, fragTexCoord);
    
    float cameraDistance = distance(vertPosition.xyz, cameraPosition.xyz);
    float nearbyAlpha = clamp(cameraDistance / 20, 0, 1);
    vec4 pixelPos = gl_FragCoord;
    if(nearbyAlpha < 1) {
        float threshold = thresholdMatrix[int(pixelPos.x) % 4][int(pixelPos.y) % 4] / 17.0;
        if(nearbyAlpha < 1 && nearbyAlpha < threshold) {
            discard;
        }
        else {
            nearbyAlpha = threshold;
        } 
    }

    outColor = (baseColor * ambient) + baseColor * cosTheta;
    outColor.x = outColor.x * colorMod.x;
    outColor.y = outColor.y * colorMod.y;
    outColor.z = outColor.z * colorMod.z;
    outColor.w = baseColor.w * colorMod.w * nearbyAlpha;
}
