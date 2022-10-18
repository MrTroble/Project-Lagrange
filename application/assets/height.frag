{
  "settings": {
    "shaderType": "fragment"
  },
  "codes": [
    {
      "code": [
        "#version 460",
        "",
        "const float maxHeight = $max_H ;",
        "const float minHeight = $min_H ;",
        "layout(location=0) out vec4 COLOR;",
        "layout(location=1) out vec4 NORMAL;",
        "layout(location=2) out float ROUGHNESS;",
        "layout(location=3) out float METALLIC;",
        "layout(location=0) in float height;",
        "vec3 hue_to_rgb(float hue){",
        "float R = abs(hue * 6.0 - 3.0) - 1.0;",
        "float G = 2.0 - abs(hue * 6.0 - 2.0);",
        "float B = 2.0 - abs(hue * 6.0 - 4.0);",
        "return clamp(vec3(R,G,B), vec3(0,0,0), vec3(1, 1, 1));}",
        "void main() {",
        "   const float clamped = (height - minHeight)/(maxHeight - minHeight);",
        "   COLOR = vec4(hue_to_rgb(clamped*2.0f+0.15f), 1);",
        "   NORMAL = vec4(1, 1, 1, 1);",
        "   ROUGHNESS = 0;",
        "   METALLIC = 0;",
        "}"
      ]
    }
  ]
}