#pragma once

#include "../Stdbase.hpp"
#include "../util/VectorUtil.hpp"

extern bool FIRST_MOUSE_BUTTON;
extern bool SECOND_MOUSE_BUTTON;
extern bool THIRED_MOUSE_BUTTON;

extern glm::vec2 last_pos;
extern std::vector<void(*)(glm::vec2, glm::vec2)> mouse_handler;

SINCE(0, 0, 3)
void inputupdate(glm::vec2 pos);

SINCE(0, 0, 3)
void addListener(void (*mouse)(glm::vec2, glm::vec2));