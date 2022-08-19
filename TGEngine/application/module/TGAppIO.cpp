#include "TGAppIO.hpp"
#include "../TGApp.hpp"

bool isFocused()
{
	return guiModul->focused;
}

void TGAppIO::tryUpdateY(float delta)
{
	if (this->changeY != 0)
	{
		guiModul->currentY += this->changeY * 0.1 * delta;
		makeVulkan();
		this->changeY = 0;
	}
}
