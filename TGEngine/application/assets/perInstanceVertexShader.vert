{
  "settings": {
    "shaderType": "vertex"
  },
  "codes": [
    {
      "code": [
        "#version 450 ",
        "#extension GL_KHR_vulkan_glsl: enable ",
        " ",
        "struct Cell { ",
        "   vec3 point[8]; ",
        "}; ",
        " ",
        "layout(std140,binding=0) readonly buffer CELL_UBO {  ",
        "   Cell cells[17483648]; ",
        "} cellUbo; ",
        "layout(binding=1) uniform VIEW_UBO {  ",
        "   mat4 viewMatrix; ",
        "} viewUbo; ",
        " ",
        "const int degreeX = $degree_X ; ",
        "const int stepsX  = $steps_X ; ",
        "const int maxStepsX = degreeX * stepsX; ",
        " ",
        "const int degreeY = $degree_Y ; ",
        "const int stepsY = $steps_Y ;",
        "const int maxStepsY = degreeY * stepsY; ",
        "out gl_PerVertex { ",
        "   vec4 gl_Position; ",
        "}; ",
        "void main() { ",
        "   Cell cell = cellUbo.cells[gl_InstanceIndex]; ",
        "   gl_Position = vec4(cell.point[gl_VertexIndex], 0);",
        "} "
      ]
    }
  ]
}