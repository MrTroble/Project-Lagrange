#version 460
#extension GL_ARB_separate_shader_objects : enable

uniform Mblock {
   vec2 multiplier;
};

layout(location = 0) in vec3 posIn;
layout(location = 1) in vec3 colorIn;
layout(location = 0) out vec3 colorOut;

out gl_PerVertex{
   vec4 gl_Position;
};

void main(){
   gl_Position = vec4(posIn.x * multiplier.x, posIn.y * multiplier.y, posIn.z, 1);
   colorOut = colorIn;
}