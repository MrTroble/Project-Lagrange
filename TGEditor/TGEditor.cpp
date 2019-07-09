#include "TGEditor.hpp"
#include <io/Font.hpp>

tge::gmc::Mesh mesh;
tge::gmc::Mesh mesh2;
tge::gmc::Camera camera;
tge::gmc::Mesh map;
UIEntity entity = UIEntity(TOP_LEFT, { 0.3, 0.15 });

int main() {
	Editor editor = Editor();
	std::cout << "Starting Editor" << std::endl;
	tge::fnt::Font arial = tge::fnt::Font("resource\\arial.ttf", 40);

	Texture texture1 = Texture("resource\\test_logo.png");
	//Texture texture2 = Texture("resource\\ODST_Helmet.png");

	UIEntity entity2 = UIEntity(CENTER, glm::vec2(0.1, 0.1));
	entity2.setOffset(CENTER);
	entity.addChildren(&entity2);

	entity.setOffset(CENTER);

	UITextureComponent textureComponent1 = UITextureComponent(&texture1, glm::vec4(1.0f));
	entity.addDrawable(&textureComponent1);

	//UITextureComponent textureComponent2 = UITextureComponent(&texture2, glm::vec4(1.0f));
	//entity2.addDrawable(&textureComponent2);

	UITextComponent text = UITextComponent(&arial, "Test", glm::vec4(1.0f));
	entity.addDrawable(&text);

	ui_scene_entity.addChildren(&entity);

	tg_model::load("resource\\map2.fbx", &map);
	//tg_model::load("resource\\Chair.fbx", &mesh);
	//tg_model::load("resource\\hall.fbx", &mesh2);

	tge::gmc::Actor actor;
	actor.mesh = &map;
	tge::gmc::actors.push_back(&actor);

	initTGEngine(&editor.main_window, &drawloop, &init);
	std::cout << "Clean exit! Bye :wave:!" << std::endl;
	return 0;
}

void init() {
	//createActor(&mesh2)->preScale(0.015, 0.08, 0.005)->prePos(0, -240, 840)->applyPretransform();
	//createActor(&mesh)->preRotate(PI / 2, 1.0f, 0, 0)->preScale(0.5, 0.5, 0.5)->applyPretransform();
	//createActor(&mesh);
	tge::gmc::createFirstPersonCamera(&camera);

}

void drawloop(IndexBuffer* ibuffer, VertexBuffer* vbuffer) {

}
