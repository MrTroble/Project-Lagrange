#pragma once

#include "Stdbase.hpp"
#include "App.hpp"
#include "pipeline/Pipeline.hpp"
#include "pipeline/Draw.hpp"
#include "gamecontent/Actor.hpp"
#include "ui/UIEntity.hpp"
#include "gamecontent/UIMaterial.hpp"
#include "gamecontent/Camera.hpp"

SINCE(0, 0, 1)
void initTGEngine(Window* window, void(*draw)(IndexBuffer*, VertexBuffer*), void(*init)(void));