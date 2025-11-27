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

float specularExponent = 5;
vec3 specularReflectionColor = vec3(0.1f, 0.1f, 0.0f); 
float ambientLightIntensity = 0.1f;

vec3 GetNormal()
{
    // if ((textureMask & (1 << 1)) == 0)
    // {
        return normalize(inNormal);
    // }
    // else
    // {
    //     vec3 normal = (texture(normalMap, inTextureCoords) * 2 - 1).xyz;
    //     mat3 TBN = mat3(inTangent, inBitangent, inNormal);
    //     return normalize(TBN * normal);
    // }
}

vec3 GetDiffuseColor()
{
    if ((textureMask & (1 << 0)) == 0)
    {
        return color.xyz;
    }
    else
    {
        vec2 uv = vec2(inTextureCoords.x, 1.0 - inTextureCoords.y);
		return texture(diffuseTexture, uv).xyz;
    }
    
}

vec3 ComputeSpecularReflection(vec3 viewDirection, vec3 lightDirection, vec3 normal)
{
    bool shouldSpecularShow = dot(normal, lightDirection) > 0;

    vec3 reflectedRay = reflect(-lightDirection, normal); 
    float specCosAngle = max(dot(reflectedRay, viewDirection), 0.0);
    return pow(specCosAngle, specularExponent) * specularReflectionColor * float(shouldSpecularShow);

}

vec3 ComputeDiffuseReflection(vec3 lightDirection, vec3 normal, vec3 lightColor, float lightIntensity)
{
    return lightIntensity * lightColor * max(dot(lightDirection, normal), 0);
}


void main() 
{
    vec3 finalBase = GetDiffuseColor();
    vec2 centered = inTextureCoords * 2.0 - 1.0;
    float dist = length(centered);

    float radius = 1.0;     // circle radius
    float edge   = 0.015;   // soft edge for AA
    float border = 0.06;    // border thickness

    float alpha = smoothstep(radius, radius - edge, dist);

    vec3 borderColor = vec3(0.0, 0.0, 0.0);

    float borderMask = smoothstep(radius - border, radius - border + edge, dist) * 
                       (1.0 - smoothstep(radius - edge, radius, dist));

    vec3 finalColor = mix(finalBase, borderColor, borderMask);
    if (alpha <= 0.001) discard;

    fragColor = vec4(finalColor, alpha);
}