{
  "settings": {
    "shaderType": "vertex"
  },
  "codes": [
    {
      "code": [
        "#version 450",
        "#extension GL_KHR_vulkan_glsl: enable",
        "const int maxStepsX = $stepsX ;",
        "const int maxStepsY = $stepsY ;",
        "struct Cell {",
        "   vec3 centerOfCell;",
        "   float height[maxIndex];",
        "};",
        "layout(std140,binding=0) readonly buffer CELL_UBO { ",
        "   Cell cells[17483648];",
        "} cellUbo;",
        "layout(binding=1) uniform VIEW_UBO { ",
        "   mat4 viewMatrix;",
        "} viewUbo;",
        "out gl_PerVertex {",
        "   vec4 gl_Position;",
        "};",
        "void main() {",
        "   const Cell cell = cellUbo.cells[gl_InstanceIndex];",
        "   vec2 position = vec2(gl_VertexIndex % maxStepsX, floor(gl_VertexIndex / maxStepsX));",
        "   gl_Position = vec4(position, cell.point[gl_VertexIndex], 0);",
        "}"
      ]
    }
  ]
}