#version 450
#extension GL_KHR_vulkan_glsl: enable

struct Cell {
   vec4 centerOfCell;
   vec3 point[8];
   int degreeX;
   int degreeY;
   int stepsX;
   int stepsY;
};

layout(binding=0) uniform CELL_UBO { 
   Cell cells[147483648];
} cellUbo;
layout(binding=1) uniform VIEW_UBO { 
   mat4 viewMatrix;
} viewUbo;

layout(location=1) in int cellIndex;

out gl_PerVertex {
   vec4 gl_Position;
};

const vec2[] GRID = { vec2(0,0) };

void main() {
   Cell cell = cellUbo.cells[cellIndex];
   vec2 gridPosition = GRID[gl_VertexIndex];
   gl_Position = vec4(gridPosition, 0, 0);
}
