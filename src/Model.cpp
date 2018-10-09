/*
* Vulkan Invaders - 3D model loading class
*
* Copyright (C) 2018 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "Model.h"
#include "tiny_obj_loader.h"

Model::Model(std::string id, std::string filename, VulkanDevice &device, VkQueue queue, VkCommandPool commandPool) : device(device), queue(queue)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());

	if (!err.empty()) {
		std::cerr << "Could not load model \"" << filename << "\":" << err << std::endl;
		return;
	}

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	for (size_t s = 0; s < shapes.size(); s++) {
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];
			for (size_t v = 0; v < fv; v++) {
				Vertex vertex{};
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				vertex.position = glm::make_vec3(&attrib.vertices[3 * idx.vertex_index]);
				vertex.normal = glm::make_vec3(&attrib.normals[3 * idx.normal_index]);
				vertex.uv = glm::make_vec2(&attrib.texcoords[2 * idx.texcoord_index]);
				vertices.push_back(vertex);
				indices.push_back(index_offset + v);
			}
			index_offset += fv;
			shapes[s].mesh.material_ids[f];
		}
	}

	const size_t vbSize = vertices.size() * sizeof(Vertex); 
	const size_t ibSize = indices.size() * sizeof(uint32_t);

	VulkanDeviceBuffer vertexStaging = device.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vbSize, vertices.data());
	VulkanDeviceBuffer indexStaging = device.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ibSize, indices.data());

	vertexBuffer = device.createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbSize);
	indexBuffer = device.createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);

	VkCommandBuffer copyCmd;

	VkCommandBufferAllocateInfo cbAllocateInfo{};
	cbAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbAllocateInfo.commandPool = commandPool;
	cbAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbAllocateInfo.commandBufferCount = 1;
	VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cbAllocateInfo, &copyCmd));

	VkCommandBufferBeginInfo cbBeginInfo{};
	cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	VK_CHECK_RESULT(vkBeginCommandBuffer(copyCmd, &cbBeginInfo));
	VkBufferCopy copyRegion = {};
	copyRegion.size = vbSize;
	vkCmdCopyBuffer(copyCmd, vertexStaging.buffer, vertexBuffer.buffer, 1, &copyRegion);
	copyRegion.size = ibSize;
	vkCmdCopyBuffer(copyCmd, indexStaging.buffer, indexBuffer.buffer, 1, &copyRegion);
	VK_CHECK_RESULT(vkEndCommandBuffer(copyCmd));

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &copyCmd;

	VkFenceCreateInfo fenceInfoCI{};
	fenceInfoCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkFence fence;
	VK_CHECK_RESULT(vkCreateFence(device, &fenceInfoCI, nullptr, &fence));

	VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence));
	VK_CHECK_RESULT(vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000000));

	vkDestroyFence(device, fence, nullptr);

	vkDestroyBuffer(device, vertexStaging.buffer, nullptr);
	vkFreeMemory(device, vertexStaging.memory, nullptr);
	vkDestroyBuffer(device, indexStaging.buffer, nullptr);
	vkFreeMemory(device, indexStaging.memory, nullptr);
}

Model::~Model()
{
	vkDestroyBuffer(device, vertexBuffer.buffer, nullptr);
	vkFreeMemory(device, vertexBuffer.memory, nullptr);
	vkDestroyBuffer(device, indexBuffer.buffer, nullptr);
	vkFreeMemory(device, indexBuffer.memory, nullptr);
}
