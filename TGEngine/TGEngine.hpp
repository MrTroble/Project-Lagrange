#pragma once

#include "Stdbase.hpp"
#include "App.hpp"
#include "pipeline/Pipeline.hpp"
#include "pipeline/Draw.hpp"
#include "gamecontent/Actor.hpp"
#include "ui/UIEntity.hpp"
#include "gamecontent/camera/Camera.hpp"

extern VertexBuffer vertexBuffer;
extern IndexBuffer indexBuffer;

SINCE(0, 0, 5)
void initEngine();

SINCE(0, 0, 1)
void startTGEngine();