#version 330

// Input
// TODO(student): Get vertex attributes from each location

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_texture;
layout(location = 3) in vec3 v_color;


// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 Color;
uniform int hit;

uniform vec3 light_position;
uniform vec3 light_position2;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

out vec3 world_position;
out vec3 world_normal;


// Output
// TODO(student): Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_texture;
out vec3 frag_color;




void main()
{
    float displacementFactor = tan(hit * 0.4);
    vec3 center = vec3(0.2, 0.1 ,-0.2);
    vec3 displacedPos = v_position; 
    if (hit > 0 && hit < 4) {
       
        vec3 displacementVec = v_position - center;
        float distance = length(displacementVec);
    
        if (distance < 1.6) {
            displacedPos = displacedPos + (displacementFactor * vec3(-0.3,0.14,0.2)) ; 
        }
    }
    if (hit >3 )
    {
    vec3 displacementVec = v_position - center;
        float distance = length(displacementVec);
    
        if (distance < 1.6) {
            displacedPos = displacedPos + (tan(3*0.4) * vec3(-0.3,0.14,0.2)) ; 
        }

    }
    frag_position = displacedPos; 
    frag_normal = v_normal;
    frag_texture = v_texture;
     world_position = vec3 (Model * vec4(v_position, 1));
     world_normal =  normalize (mat3 (Model) * v_normal);

    



    frag_color = Color;


	

    gl_Position = Projection * View * Model * vec4(frag_position , 1.0);


}
