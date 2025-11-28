#version 460 core 
layout (location = 0) uniform mat4 modelMatrix; 
layout (location = 7) uniform int lightsNum; 
layout (location = 4) uniform vec3 cameraPos; 
layout (location = 5) uniform vec4 color;
layout (location = 6) uniform int textureMask;

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
    float linearAttenuation;
    float quadraticAttenuation;
};
layout (binding = 0, std430) buffer PointLightBuffer 
{
    PointLight pointLights[];
};

layout (binding = 0) uniform sampler2D diffuseTexture;
layout (binding = 1) uniform sampler2D normalMap;

layout (location = 0) in vec3 inFragPosition;
layout (location = 1) in vec3 inTangent;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTextureCoords;
layout (location = 4) in vec3 inBitangent;

out vec4 fragColor; 

vec3 GetDiffuseColor()
{
    vec2 uv = vec2(inTextureCoords.x, 1.0 - inTextureCoords.y);
    return texture(diffuseTexture, uv).xyz;
}

void main() 
{
    vec3 finalBase = GetDiffuseColor();
    vec2 centered = inTextureCoords * 2.0 - 1.0;
    float dist = length(centered);

    float radius = 1.0;     // circle radius
    float edge   = 0.075;   // soft edge for AA
    float border = 0.07;    // border thickness

    float alpha = smoothstep(radius, radius - edge, dist);

    vec3 borderColor = vec3(0.0, 0.2, 0.0);

    float borderMask = smoothstep(radius - border, radius - border + edge, dist) * 
                       (1.0 - smoothstep(radius - edge, radius, dist));

    vec3 finalColor = mix(finalBase, borderColor, borderMask);
    if (alpha <= 0.3) discard;

    fragColor = vec4(finalColor, alpha);
}