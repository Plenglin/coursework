#version 450 
#extension GL_ARB_shader_storage_buffer_object : require

layout(local_size_x = 1024, local_size_y = 1) in;	


layout (std430, binding=0) volatile buffer shader_data
{ 
  vec4 dataA[1024];
 
};

uniform int sizeofbuffer;

void main() 
	{

	uint index = gl_LocalInvocationID.x;	
	
	vec4 data = dataA[index];

	dataA[index].x = int(data.x)*3;

	}