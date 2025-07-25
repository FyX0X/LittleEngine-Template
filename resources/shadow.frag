#version 330 core
out vec4 FragColor;

uniform vec3 uShadowColor;

void main()
{
	FragColor = vec4(uShadowColor, 1.0); // Default to black for the shadow mask
	
	// Set shadow color purple for testing
	FragColor = vec4(0.5, 0, 0.5, 1.0); // Purple shadow color
}