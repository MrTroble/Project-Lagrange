#version 450
#extension GL_KHR_vulkan_glsl: enable

struct Cell {
   vec3 point[8];
};

layout(binding=0) uniform CELL_UBO { 
   Cell cells[147483648];
} cellUbo;
layout(binding=1) uniform VIEW_UBO { 
   mat4 viewMatrix;
} viewUbo;

const int degreeX = $degree_X ;
const int stepsX  = $steps_X ;
const int maxStepsX = degreeX * stepsX;

const int degreeY = $degree_Y ;
const int stepsY = $steps_Y ;
const int maxStepsY = degreeY * stepsY;

layout(location=0) in int cellIndex;

out gl_PerVertex {
   vec4 gl_Position;
};

void main() {
   Cell cell = cellUbo.cells[cellIndex];
   vec2 position = vec2(gl_VertexIndex % maxStepsX, floor(gl_VertexIndex / maxStepsX));
   gl_Position = vec4(position, 0, 0);
}
 