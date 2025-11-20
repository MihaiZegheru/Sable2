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
    float intenisty;
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

float specularExponent = 8;
vec3 specularReflectionColor = vec3(0.2f); 
float ambientLightIntensity = 0.2f;

vec3 GetNormal()
{
    if ((textureMask & (1 << 1)) == 0)
    {
        return normalize(inNormal);
    }
    else
    {
        vec3 normal = (texture(normalMap, inTextureCoords) * 2 - 1).xyz;
        mat3 TBN = mat3(inTangent, inBitangent, inNormal);
        return normalize(TBN * normal);
    }
}

vec3 GetDiffuseColor()
{
    if ((textureMask & (1 << 0)) == 0)
    {
        return color.xyz;
    }
    else
    {
        return texture(diffuseTexture, inTextureCoords).xyz; 
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
    vec3 normal = GetNormal();
    vec3 diffuseColor = GetDiffuseColor();
    vec3 ambient = ambientLightIntensity * diffuseColor;

    vec3 lightPos;
    vec3 specular = vec3(0); 
    vec3 diffuse = vec3(0);
    vec3 viewDirection = normalize(cameraPos - inFragPosition); 

    for (int i = 0; i < lightsNum; i++)
    {
        lightPos = pointLights[i].position;
        vec3 lightDirection = normalize(lightPos - inFragPosition); 

        float dist = distance(lightPos, inFragPosition);
        float attenuation = 1.f / (1.f + pointLights[i].linearAttenuation * dist + pointLights[i].quadraticAttenuation * dist * dist);

        specular += ComputeSpecularReflection(viewDirection, lightDirection, normal) * attenuation;
        diffuse += ComputeDiffuseReflection(lightDirection, normal, pointLights[i].color, pointLights[i].intenisty) * attenuation;
        // diffuse += ComputeDiffuseReflection(lightDirection, normal, pointLights[i].color, 1);
        // diffuse += ComputeDiffuseReflection(lightDirection, normal, vec3(0.7, 0, 0), 1);
    }
    diffuse *= diffuseColor;

    fragColor = vec4(ambient + specular + diffuse, 1); 
	// fragColor = vec4(lightPos, 1);
	// fragColor = vec4(pointLights[0].color, 1);
	// fragColor = vec4(1.f * lightsNum, 1);
    // fragColor = vec4(pointLights[0].color, 1); 

        // ambient
    // float ambientStrength = 0.1;
    // vec3 ambient = ambientStrength * ambientLightColor;s
  	
    // // diffuse 
    // vec3 lightDir = normalize(lightPos - inFragPosition);
    // float diff = max(dot(normal, lightDir), 0.0);
    // vec3 diffuse = diff * ambientLightColor;
    
    // // specular
    // float specularStrength = 0.5;
    // vec3 viewDir = normalize(- cameraPos + inFragPosition);
    // vec3 reflectDir = reflect(-lightDir, normal);  
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // vec3 specular = specularStrength * spec * ambientLightColor;  
        
    // vec3 result = (ambient + diffuse + specular) * diffuseColor;
    // fragColor = vec4(result, 1.0);
}
