#pragma once

#include "UIEntity.hpp"

namespace tge {
	namespace ui {
		/*
         * 	Component class that holds the actual draw and update calls
         */
		SINCE(0, 0, 4)
		class UIDrawable {

		protected:
			// Parent this belongs to
			// if null this is unbound
			UIEntity* parent = nullptr;
			// Anchor to calculate position updates
			Anchor anchor = CENTER;
			// cached value
			glm::vec2 cachedPosition = glm::vec2(0.0f);

		private:
			bool enabled = true; // if enabled this will be draw it will still recieve updates

		public:
			UIDrawable(Anchor anchor) : anchor(anchor) {}

			virtual void draw(IndexBuffer* index, VertexBuffer* vertex); // default implementation, needs override
			virtual void init(); // default implementation, needs override

			void update();

			void onAddTo(UIEntity* parent);
			void onRemoveFrom(UIEntity* parent);

			// See enabled
			bool isEnabled();
			void setEnabled(bool enabled);

		};


	}
}