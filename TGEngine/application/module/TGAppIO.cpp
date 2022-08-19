#include "TGAppIO.hpp"
#include "../TGApp.hpp"

bool isFocused()
{
	return guiModul->focused;
}

void TGAppIO::tryUpdateY()
{
	if (this->changeY != 0)
	{
		guiModul->currentY += this->changeY * 0.01;
		makeVulkan();
		this->changeY = 0;
	}
}
