#pragma once

#include "Memory.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../../../stb/stb_image.h"
#include "../../vlib/VulkanImage.hpp"
#include "../../vlib/VulkanBuffer.hpp"
#include "../../util/VectorUtil.hpp"
#include "../Descriptors.hpp"

#define AUTO_MIPMAP 0xFFFFFF

namespace tge {
	namespace tex {

		/*
		 * Holder struct for sampler
		 *  -> API independency layer
		 */
		SINCE(0, 0, 4)
		typedef VkSampler Sampler;

		extern VkSampler defaultImageSampler;  // The default sampler if none is provided

		/*
		 * Wrapper for all textures
		 *   -> API independent
		 */
		SINCE(0, 0, 4)
			class Texture {

			public:
				Sampler sampler = VK_NULL_HANDLE;// holds a custom sampler
				uint8_t* imageData = nullptr; // stores the image data -> see stb

			private:
				const char* textureName = nullptr; // name
				uint32_t miplevels = AUTO_MIPMAP; // the maximum mipmap levels -> currently not changable
				// TODO changable

				int width = 0; // stores the width -> see stb
				int height = 0; // stores the height -> see stb
				int channel = 0; // stores the channel -> see stb

#ifdef VULKAN_H_ // VULKAN ONLY
				VkImage image; // The image vulkan representation
				VkImageView imageView; // The view of the image ... to view the image LOL
				VkBuffer buffer; // Buffer to store the image in befor it is transfered to the actual image -> deleted after transfer

				VkDeviceMemory imageMemory; // The bound image memory
				VkDeviceMemory bufferMemory; // The bound buffer memory -> deleted after transfer

				VkFormat imageFormat; // Auto generated!
#endif // VULKAN_H_

			public:
				Texture(const char* textureName); // Loads texture from file
				Texture(int width = 0, int height = 0); // Costum texture

				void initTexture(); // inits all image resources
				void load(VkCommandBuffer buffer); // loads the image and it's according mip levels

				int getWidth(); // gets the width
				int getHeight(); // gets the height

#ifdef VULKAN_H_ // VULKAN ONLY
				void initVulkan();

				void* map(uint32_t size); // Maps the buffer memory
				void unmap(); // Unmaps buffer memory
				void updateDescriptor(); // Updates the predifined descriptor
				void queueLoading(VkCommandBuffer buffer); // Queues the loading of the image data, from the buffer to the image, to the singletime command buffer
				void generateMipmaps(VkCommandBuffer buffer); // Queues the generation of mip maps

				void dispose(); // Disposes unneeded resources
				void destroy(); // Destroys the whole texture
#endif // VULKAN_H_

		};

		extern std::vector<Texture*> textures; // All textures
		extern Descriptor textureDescriptor; // The descritor for the textures
		extern VkFormat defaultImageFormat; // Auto querryed vulkan format

		/*
		 * Inits all textures
		 */
		SINCE(0, 0, 2)
			void initAllTextures();

		/*
		 * Creates a custom sampler
		 */
		SINCE(0, 0, 4)
			void createSampler(Sampler* sampler);

		/*
		 * Destroys all textures
		 */
		SINCE(0, 0, 2)
			void destroyAllTextures();
	}
}