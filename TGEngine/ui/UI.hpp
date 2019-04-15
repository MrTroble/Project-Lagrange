#pragma once

#include "../util/VectorUtil.hpp"
#include "../pipeline/buffer/IndexBuffer.hpp"
#include "../pipeline/buffer/VertexBuffer.hpp"
#include "../drawlib/DrawLib.hpp"
#include "../pipeline/buffer/Texturebuffer.hpp"
#include "../io/LoadFont.hpp"
#include "../game_content/Camera.hpp"

namespace tg_ui {

	/*
	 * Defines the anchor that is used for the component
	 */
	enum Anchor {
		TOP_LEFT, TOP_CENTER, TOP_RIGHT,
		CENTER_LEFT, CENTER, CENTER_RIGHT,
		BOTTOM_LEFT, BOTTOM_CENTER, BOTTOM_RIGHT
	};

	class UIComponent; // See defenition below

	/*
	 * 	Entity class that holds draw and update components
	 */
	SINCE(0, 0, 4)
	class UIEntity {

	public:
		UIEntity(glm::vec2 position, glm::vec2 extent);
		glm::vec2 local_position;
		glm::vec2 extent;

		void addComponent(UIComponent* component);
		void addChildren(UIEntity* children);
		void removeComponent(UIComponent* component);
		void removeChildren(UIEntity* children);

		void draw(IndexBuffer* index, VertexBuffer* vertex);
		void update(int mouse_x, int mouse_y);
		void init();

		glm::vec2 getPosition();
		glm::vec2 UIEntity::getRenderPosition();
		glm::vec2 UIEntity::getRenderExtent();

	protected:
		UIEntity* parent = nullptr;

		std::vector<UIComponent*> components;
		std::vector<UIEntity*> children;

		void onAddTo(UIEntity* parent);
		void onRemoveFrom(UIEntity* parent);
	};

	/*
     * 	Component class that holds the actual draw and update calls
     */
	SINCE(0, 0, 4)
	class UIComponent {

	public:
		UIComponent(Anchor anchor) : anchor(anchor) {}

		virtual void draw(IndexBuffer* index, VertexBuffer* vertex); // default implementation, needs override
		virtual void update(int mouse_x, int mouse_y); // default implementation, needs override
		virtual void init(); // default implementation, needs override

		void onAddTo(UIEntity* parent);
		void onRemoveFrom(UIEntity* parent);

	protected:
		UIEntity* parent;
		Anchor anchor;
	};

	extern UIEntity ui_scene_entity;
}