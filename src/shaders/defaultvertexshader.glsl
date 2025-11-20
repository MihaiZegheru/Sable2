#version 460 core 
layout (location = 0) in vec4 inPosition; 
layout (location = 1) in vec4 inTangent; 
layout (location = 2) in vec4 inNormal; 
layout (location = 3) in vec2 inTextureCoords; 

layout (location = 0) uniform mat4 modelMatrix; 
layout (location = 1) uniform mat4 viewMatrix; 
layout (location = 2) uniform mat4 projectionMatrix; 
layout (location = 3) uniform vec3 lightPos; 

layout (location = 0) out vec3 position; 
layout (location = 1) out vec3 tangent;
layout (location = 2) out vec3 normal; 
layout (location = 3) out vec2 textureCoords; 
layout (location = 4) out vec3 bitangent;

void main() 
{ 
   gl_Position = projectionMatrix * viewMatrix * modelMatrix * inPosition; 
   textureCoords = vec2(inTextureCoords.x, 1 - inTextureCoords.y); 
   normal = normalize(mat3(transpose(inverse(modelMatrix))) * inNormal.xyz); 
   position = (modelMatrix * inPosition).xyz; 
   tangent = normalize((modelMatrix * inTangent).xyz);
   bitangent = normalize(cross(normal, tangent));
}
