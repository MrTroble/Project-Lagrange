#include "Mouse.hpp"

glm::vec2 last_pos;
std::vector<void(*)(glm::vec2, glm::vec2)> mouse_handler;

bool FIRST_MOUSE_BUTTON;
bool SECOND_MOUSE_BUTTON;
bool THIRED_MOUSE_BUTTON;

void inputupdate(glm::vec2 pos) {
	glm::vec2 delta_pos = pos - last_pos;
	last_pos = pos;
	for each(void (*ptr)(glm::vec2, glm::vec2) in mouse_handler) {
		ptr(pos, delta_pos);
	}
}

void addListener(void(*mouse)(glm::vec2, glm::vec2)) {
	TG_VECTOR_APPEND_NORMAL(mouse_handler, mouse)
}