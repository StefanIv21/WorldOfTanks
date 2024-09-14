#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 frag_pos;
in vec3 frag_normal;
in vec3 frag_text;
in vec3 frag_color;

uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 light_position2;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
in vec3 world_position;
in vec3 world_normal;


// Output
layout(location = 0) out vec4 out_color;
uniform int hit;

void main()
{
   
    vec3 L = normalize( light_position - world_position );
    vec3 V = normalize( eye_position - world_position );
    vec3 H = normalize( L + V );
    float light;

    float ambient_light = 0.3;
     float diffuse_light = material_kd * max (dot(world_normal, L), 0);
    float specular_light = 0;
     float cut_off = radians(50);
	float spot_light = dot(-L, light_direction);
	float spot_light_limit = cos(cut_off);

	// Quadratic attenuation
	float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
	float light_att_factor = pow(linear_att, 2);

    if(spot_light > spot_light_limit )
    {
        light  = ambient_light + light_att_factor * (diffuse_light + specular_light);
	}
    else if( spot_light < spot_light_limit)
    {
        light = ambient_light;
        
    }

     L = normalize( light_position2 - world_position );
     H = normalize( L + V );
    float light2;

    diffuse_light = material_kd * max (dot(world_normal, L), 0);
    specular_light = 0;
      cut_off = radians(50);
	 spot_light = dot(-L, light_direction);
	 spot_light_limit = cos(cut_off);

	// Quadratic attenuation
	 linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
	 light_att_factor = pow(linear_att, 2);

    if(spot_light > spot_light_limit )
    {
        light2  = ambient_light + light_att_factor * (diffuse_light + specular_light);
	}
    else if( spot_light < spot_light_limit)
    {
        light2 = ambient_light;
        
    }

   if (hit < 4)
   {
   out_color = vec4(((frag_color*light2) + (frag_color*light))  + hit *vec3(-0.2,0,0.0) ,1);
   }
   else
   {
	out_color = vec4((frag_color*light) + (frag_color*light2)+ 3 *vec3(-0.2,0,0.0),1);
   }

 


}
