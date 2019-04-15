#include "Camera.hpp"

std::vector<Camera*> cameras_on_scene;
UniformBuffer camera_uniform;
UniformBuffer ui_camera_uniform;
float multiplier = 1;
float multiplierx = 1;
size_t active_camera = 0;

void initCameras() {
	camera_uniform = UniformBuffer(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT, 0);
	ui_camera_uniform = UniformBuffer(sizeof(glm::mat4), VK_SHADER_STAGE_VERTEX_BIT, 0);

	if (cameras_on_scene.size() > 0) tg_io::addListener(__impl_input_handle);
	if (cameras_on_scene.size() > 0) tg_io::addKeyListener(__impl_keyinput_handle);
}

void createCamera(Camera* camera) {
	TG_VECTOR_APPEND_NORMAL(cameras_on_scene, camera)
	active_camera = camera->camera_index = last_size;
}

void Camera::applyWorldRotation(double x, double y, double z, double angle) 
{
	this->world_transform = glm::rotate(this->world_transform, (float)angle, glm::vec3(x, y, z));
}

void Camera::applyWorldTranslation(double x, double y, double z)
{
	this->world_transform = glm::translate(this->world_transform, glm::vec3(x, y, z));
}

void Camera::applyWorldScale(double x, double y, double z)
{
	this->world_transform = glm::scale(this->world_transform, glm::vec3(x, y, z));
}

void Camera::applyCameraRotation(glm::vec2 in)
{
	this->rotations += in;
}

void Camera::applyCameraTranslation(double x, double y, double z)
{
	glm::quat qut = glm::angleAxis((float)(this->rotations.y * -this->speed), glm::vec3(0, 0, 1));
	this->position += glm::mat3_cast(qut) * glm::vec3((float)x, (float)y, (float)z);
}

void updateCamera(int width, int height) {
	Camera* ptr = cameras_on_scene[active_camera];
	glm::quat qut = glm::angleAxis((float)(ptr->rotations.x * ptr->speed), glm::vec3(1, 0, 0));
	qut *= glm::angleAxis((float)(ptr->rotations.y * ptr->speed), glm::vec3(0, 0, 1));
	ptr->camera = glm::mat4_cast(qut) * glm::translate(glm::mat4(1.0f), -ptr->position);
	glm::mat4 projection = glm::perspective(ptr->fov, width / (float)height, ptr->near_clip_plain, ptr->far_clip_plain);
	projection[1][1] *= -1;
	ptr->matrix = projection * ptr->camera * ptr->world_transform;
	fillUniformBuffer(&camera_uniform, &ptr->matrix, sizeof(glm::mat4));
}

void __impl_input_handle(glm::vec2 pos, glm::vec2 delta)
{
	Camera* cam = cameras_on_scene[active_camera];
	if (cam->mouse_input_handler) {
		cam->mouse_input_handler(cam, pos, delta);
	}
}

void __impl_keyinput_handle(uint16_t chr, bool down)
{
	Camera* cam = cameras_on_scene[active_camera];
	if (cam->key_input_handler) {
		cam->key_input_handler(cam, chr, down);
	}
}
