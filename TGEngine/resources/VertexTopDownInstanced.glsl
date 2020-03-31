#version 460

layout(set = 1, binding = 0) uniform DATA_BLOCK {
    mat4 basicTransform;
    vec2 uvOffsets[256];
} data;

layout(push_constant) uniform PUSH_CONST{
    mat4 localTransform;
    uint animationIndex;
} pushconst;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 uvOut;

layout(location = 2) in vec2 transform;
layout(location = 3) in vec2 scale;

out gl_PerVertex{
   vec4 gl_Position;
};

void main(){
    mat4 mat = {
        { scale.x, 0, 0, transform.x },
        { 0, scale.y, 0, transform.y },
        { 0, 0, 1.0, 0 },
        { 0, 0, 0, 1.0 }
    };
    gl_Position = (vec4(pos, 1, 1) * data.basicTransform * pushconst.localTransform * mat);
    uvOut = uv + data.uvOffsets[pushconst.animationIndex];
}