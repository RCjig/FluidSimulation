////////////////////////////////////////
// Model.glsl
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#ifdef VERTEX_SHADER

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;

out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 ModelMtx=mat4(1);
uniform mat4 ModelViewProjMtx=mat4(1);

////////////////////////////////////////
// Vertex shader
////////////////////////////////////////

void main() {
	gl_Position=ModelViewProjMtx * vec4(Position,1);
    gl_PointSize = 1.5f;

	fragPosition=vec3(ModelMtx * vec4(Position,1));
	fragNormal=vec3(ModelMtx * vec4(Normal,0));
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 LightColor=vec3(1);
uniform vec3 LightDirection=normalize(vec3(1,5,2));

uniform vec3 LightPosition1=vec3(5,-5,0);
uniform vec3 AmbientColor1=vec3(0.0, 0.05, 0.05);
uniform vec3 DiffuseColor1=vec3(0.4, 0.5, 0.5);
uniform vec3 SpecularColor1=vec3(0.04, 0.7, 0.7);
uniform float Shininess1=0.25;

uniform vec3 LightPosition2=vec3(-15,8,0);
uniform vec3 AmbientColor2=vec3(0.24725, 0.1995, 0.0745);
uniform vec3 DiffuseColor2=vec3(0.75164, 0.60648, 0.22648);
uniform vec3 SpecularColor2=vec3(0.628281, 0.555802, 0.366065);
uniform float Shininess2=0.4;

out vec3 finalColor;

////////////////////////////////////////
// Fragment shader
////////////////////////////////////////

void main() {
    
    finalColor = vec3(1.0f);
    
    // point light 1
//    vec3 normal = normalize(fragNormal);
//    vec3 eye = vec3(-fragPosition);
//    vec3 lightDir = vec3(LightPosition1 - fragPosition);
//    vec3 spec = vec3(0.0);
//
//    vec3 n = normalize(normal);
//    vec3 l = normalize(lightDir);
//    vec3 e = normalize(eye);
//
//    float intensity = max(dot(n,l), 0.0);
//    if (intensity > 0.0) {
//        vec3 h = normalize(l + e);
//        float intSpec = max(dot(h,n), 0.0);
//        spec = SpecularColor1 * pow(intSpec, Shininess1);
//    }
    
    //finalColor = max(intensity * DiffuseColor1 + spec, AmbientColor1);
    
//    // point light 2
//    lightDir = vec3(LightPosition2 - fragPosition);
//    l = normalize(lightDir);
//
//    intensity = max(dot(n,l), 0.0);
//    if (intensity > 0.0) {
//        vec3 h = normalize(l + e);
//        float intSpec = max(dot(h,n), 0.0);
//        spec = SpecularColor2 * pow(intSpec, Shininess2);
//    }
//
//    vec3 pointLight2Color = max(intensity * DiffuseColor2 + spec, AmbientColor2);
    
    // final
    //finalColor = pointLight1Color;// + pointLight2Color;
    
    /*
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance=AmbientColor + LightColor * max(0,dot(LightDirection,fragNormal));

	// Diffuse reflectance
	vec3 reflectance=irradiance * DiffuseColor;

	// Gamma correction
	finalColor = sqrt(reflectance);
     */
    
    //vec3 norm = fragNormal;
    //finalColor = vec3((norm.x + 1.0f) / 2.0f, (norm.y + 1.0f) / 2.0f, (norm.z + 1.0f) / 2.0f);
}

#endif

////////////////////////////////////////////////////////////////////////////////
