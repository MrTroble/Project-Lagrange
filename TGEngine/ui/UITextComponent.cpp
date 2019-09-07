#include "UITextComponent.hpp"

namespace tge {
	namespace ui {

		UITextComponent::UITextComponent(fnt::Font* font, char* string, glm::vec4 fontColor) {
			this->font = font;
			this->string = string;
			this->fontColor = fontColor;
		}

		void UITextComponent::draw(IndexBuffer* index, VertexBuffer* vertex) {
			if(this->index == -1) {
				gmc::RenderOffsets off;
				off.material = this->font->getMaterialIndex();
				off.offset = index->indexCount;
				off.size = (uint32_t)strlen(this->string) * 6;
				TG_VECTOR_APPEND_NORMAL(gmc::render_offset, off)
					this->index = (uint32_t)lastSize;
			}

			glm::vec2 ext = this->font->getExtent(this->string);
			glm::vec2 pos = this->parent->getPosition();
			this->font->drawString(pos, this->string, vertex, index, this->fontColor);
		}

	}
}
