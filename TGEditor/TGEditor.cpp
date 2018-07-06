#include "TGEditor.hpp"

Texture tex1;
Texture tex2;
fbxsdk::FbxMesh* mesh;

int main() {
	Editor editor = Editor();
	std::cout << "Starting Editor" << std::endl;

	mesh = load("lul.fbx");

	tex1 = {
		"ODST_Helmet.png"
	};
	tex2 = {
		"LogoTM.png"
	};
	//createTexture(&tex1);
	//createTexture(&tex2);
	initTGEngine((App*) &editor);
	return 0;
}

void drawLineH(Vertex start, float length, VertexBuffer* buffer) {
	drawRectangle(start, length, 0.002, buffer);
	drawRectangle(start, -length, 0.002, buffer);
}

void drawLineV(Vertex start, float length, VertexBuffer* buffer) {
	drawRectangle(start, 0.002, length, buffer);
	drawRectangle(start, 0.002, -length, buffer);
}

void drawGrid(VertexBuffer* buffer) {
	for (uint32_t i = 0; i < 41; i++) {
		drawLineH({ { 0, -2 + 0.1 * i, 0 }, { 0, 0, 0, 1}, {0, 0}, VK_TRUE}, 2, buffer);
	}
	for (uint32_t i = 0; i < 41; i++) {
		drawLineV({ { -2 + 0.1 * i, 0, 0 },{ 0, 0, 0, 1 },{ 0, 0 }, VK_TRUE }, 2, buffer);
	}
	drawRectangleZ({ { 0, 0, 0.15 },{ 1, 0, 0, 1 }, { 0, 0 }, VK_TRUE }, 0.002, 0.15, buffer);
	drawRectangleZ({ { 0, 0, 0.15 },{ 1, 0, 0, 1 }, { 0, 0 }, VK_TRUE }, -0.002, 0.15, buffer);
	drawRectangle({ { 0, 0.15, 0},{ 0, 1, 0, 1 }, { 0, 0 }, VK_TRUE }, 0.002, 0.15, buffer);
	drawRectangle({ { 0, 0.15, 0},{ 0, 1, 0, 1 }, { 0, 0 }, VK_TRUE }, -0.002, 0.15, buffer);
	drawRectangle({ { 0.15, 0, 0 },{ 0, 0, 1, 1 },{ 0, 0 }, VK_TRUE }, 0.15, 0.002, buffer);
	drawRectangle({ { 0.15, 0, 0 },{ 0, 0, 1, 1 },{ 0, 0 }, VK_TRUE }, 0.15, -0.002, buffer);
}

void Editor::drawloop(VertexBuffer* buffer)
{
	drawGrid(buffer);
	FBX_Dictionary::addToDraw(buffer);
	//setTexture(&tex1, nullptr);
	//drawRectangleWithTexture({ { 0.5, 0.5, 0 },{ 1, 1, 1, 1 },{ 0, 0 }, VK_TRUE }, 0.5, 0.5, buffer);
	//setTexture(&tex2, nullptr);
	//drawRectangleWithTexture({ { -0.5, -0.5, 0 },{ 1, 1, 1, 1 },{ 0, 0 }, VK_TRUE }, 0.5, 0.5, buffer);
}
