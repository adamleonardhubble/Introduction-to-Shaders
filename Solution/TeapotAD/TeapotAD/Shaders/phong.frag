#version 430

in vec3 vertPos; // Vertex position
in vec3 N; // Light normal
in vec3 lightPos; // Light position

uniform vec3 CameraPosition; // CameraPosition (uniform Vector3)

float quadratic = 0; // Float used by attenuation (enhances scale)

// Structure of lighting effecting variables
struct Lighting
{
	vec3 La; // Ambient light intensity

	vec3 Ld; // Diffuse light intensity

	vec3 Ls; // Specular light intensity
	
	float attenuationValue; // Attenuation (uniform float)
};

// Structure of material effecting variables
struct Material
{
	vec3 Ka; // Ambient reflectivity
	
	vec3 Kd; // Diffuse reflectivity

	vec3 Ks; // Specular reflectivity

	float Shininess; // Shininess (uniform float)
};

layout(location = 0) out vec4 FragColour; // Complete to a full phong shading

uniform Lighting ADS; // Uniform structure object, lighting
uniform Material Mat; // Uniform structure object, material

void main() 
{
   // Calculate the light vector
   vec3 L = normalize(lightPos - vertPos); // Light position
   vec3 V = normalize(CameraPosition - vertPos); // Viewing position (camera eye)
   vec3 R = normalize(reflect(-L, N)); // Angle of reflection (light)
    
   // Calculate Diffuse Light Intensity making sure it is not negative and is clamped 0 to 1  
   vec4 Id = vec4(Mat.Kd, 1.0) * vec4(ADS.Ld, 1.0) * max(dot(N, L), 0.0); // Diffuse light intensity
   vec4 Ia = vec4(Mat.Ka, 1.0) * vec4(ADS.La, 1.0); // Ambient light intensity
   vec4 Is = vec4(Mat.Ks, 1.0) * vec4(ADS.Ls, 1.0) * max(pow(dot(V, R), Mat.Shininess), 0.0); // Specular light intensity

   float distance = length(lightPos - vertPos); // Distance equals length when light position is taken from vertice position
   float attenuation = ADS.attenuationValue / (distance + quadratic * (distance * distance)); // Attenuation of light

   Id *= attenuation; // Multiply and equal attenuation for diffuse lighting
   Is *= attenuation; // Multiply and equal attenuation for specular lighting

   FragColour = clamp(Id + Ia + Is, 0.0, 1.0); // Multiply the Reflectivity by the Diffuse intensity, clamped (0 - 1)
}
