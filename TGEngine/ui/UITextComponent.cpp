#include "UITextComponent.hpp"

namespace tg_ui {

	UITextComponent::UITextComponent(Font * font, char * string) : UIComponent(TOP_LEFT)
	{
		this->font = font;
		this->string = string;
		this->font_color = glm::vec4(1, 1, 1, 1);
	}

	UITextComponent::UITextComponent(Font * font, glm::vec4 font_color, char * string) : UIComponent(TOP_LEFT)
	{
		this->font = font;
		this->string = string;
		this->font_color = font_color;
	}

	void UITextComponent::draw(IndexBuffer * index, VertexBuffer * vertex)
	{
		this->font->drawString(glm::vec3(this->parent->getPosition(), 0.1f), this->font_color, this->string, vertex, index);
	}

}
