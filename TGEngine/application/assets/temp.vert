#version 450
#extension GL_KHR_vulkan_glsl: enable

const int maxStepsX = $steps_X ;
const int maxStepsY = $steps_Y ;
const int maxPoints = $allpoints_n ;

layout(std140,binding=0) readonly buffer CELL_UBO { 
   vec4 cells[maxPoints];
} cellUbo;
layout(binding=1) uniform VIEW_UBO { 
   mat4 viewMatrix;
} viewUbo;

out gl_PerVertex {
   vec4 gl_Position;
};

void main() {
   const int bufferIndex = gl_InstanceIndex * 4 + gl_VertexIndex;
   gl_Position = viewUbo.viewMatrix * cellUbo.cells[bufferIndex];
}
 