#include "model.h"

#include "mesh.h"
#include "material.h"
#include "texture_lib.h"
#include "core/device.h"
#include "resources/buffers/vertex_buffer.h"
#include "resources/memory/memory_allocator.h"
#include "resources/descriptors/descriptor_set_layout.h"
#include "resources/descriptors/descriptor_pool.h"
#include "resources/descriptors/descriptor_writer.h"
#include "graphics/command_pool.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdexcept>

namespace vkcommon {

    Model::Model(const Device& device, MemoryAllocator& allocator)
        : m_deviceRef(device)
        , m_allocatorRef(allocator) {
    }

    Model::Model(Model&& other) noexcept
        : m_deviceRef(other.m_deviceRef)
        , m_allocatorRef(other.m_allocatorRef)
        , m_meshes(std::move(other.m_meshes)) {
    }

    Model& Model::operator=(Model&& other) noexcept {
        if (this != &other) {
            m_meshes = std::move(other.m_meshes);
        }
        return *this;
    }

    void Model::draw(
        VkCommandBuffer commandBuffer,
        uint32_t currentFrame,
        VkPipelineLayout pipelineLayout) {
        for (const auto& mesh : m_meshes) {
            mesh->draw(commandBuffer, currentFrame, pipelineLayout);
        }
    }

    void Model::createDescriptor(DescriptorPool& pool, const DescriptorSetLayout& materialLayout, uint32_t framesInFlight)
    {
        for (const auto& mesh : m_meshes) {
            if (mesh->m_material) {
                mesh->m_material->createPropertiesUBO(framesInFlight);
                mesh->m_material->initDescriptorSets(pool, materialLayout, framesInFlight);
            }
        }
    }

    void Model::updateProperties(uint32_t currentFrame)
    {
        for (const auto& mesh : m_meshes) {
            if (mesh->m_material) {
                mesh->m_material->updateProperties(currentFrame);
            }
        }
    }

    void Model::loadFromFile(
        const std::filesystem::path& path,
        TextureLibrary& textureLib,
        const CommandPool& cmdPool) {

        // Initialize Assimp importer with common post-processing steps
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.string(),
            aiProcess_Triangulate |            // Ensure all primitives are triangles
            aiProcess_GenSmoothNormals |      // Generate smooth normals
            aiProcess_FlipUVs |               // Flip texture coordinates
            aiProcess_CalcTangentSpace);      // Generate tangents/bitangents

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error("Failed to load model: " + path.string() + "\n" + importer.GetErrorString());
        }

        // Start recursive loading from root node
        loadNode(scene->mRootNode, scene, textureLib, cmdPool, path.parent_path());
    }

    void Model::loadNode(
        const aiNode* node,
        const aiScene* scene,
        TextureLibrary& textureLib,
        const CommandPool& cmdPool,
        const std::filesystem::path& modelPath) {

        // Process all meshes in the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene, textureLib, cmdPool, modelPath);
        }

        // Process all child nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            loadNode(node->mChildren[i], scene, textureLib, cmdPool, modelPath);
        }
    }

    void Model::processMesh(
        const aiMesh* mesh,
        const aiScene* scene,
        TextureLibrary& textureLib,
        const CommandPool& cmdPool,
        const std::filesystem::path& modelPath) {

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // Process vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};

            // Position
            vertex.pos = {
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            };

            // Normal
            if (mesh->HasNormals()) {
                vertex.normal = {
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                };
            }

            // Texture coordinates
            if (mesh->mTextureCoords[0]) {
                vertex.texCoord = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                };

                // Tangent
                vertex.tangent = {
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z
                };

                // Bitangent
                vertex.bitangent = {
                    mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z
                };
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Create mesh
        auto newMesh = std::make_shared<Mesh>(m_deviceRef, m_allocatorRef);
        newMesh->createVertexBuffer(vertices, indices, cmdPool);

        // Process material
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // Get material properties
            aiColor3D color;
            float value;

            if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
                newMesh->m_material->m_properties.ambientColor = { color.r, color.g, color.b, 1.0f };
            }
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
                newMesh->m_material->m_properties.diffuseColor = { color.r, color.g, color.b, 1.0f };
            }
            if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
                newMesh->m_material->m_properties.specularColor = { color.r, color.g, color.b, 1.0f };
            }
            if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
                newMesh->m_material->m_properties.emissiveColor = { color.r, color.g, color.b, 1.0f };
            }
            if (material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
                newMesh->m_material->m_properties.shininess = value;
            }
            if (material->Get(AI_MATKEY_OPACITY, value) == AI_SUCCESS) {
                newMesh->m_material->m_properties.opacity = value;
            }
            if (material->Get(AI_MATKEY_REFRACTI, value) == AI_SUCCESS) {
                newMesh->m_material->m_properties.refractiveIndex = value;
            }

            // Load textures
            loadMaterialTextures(material, textureLib, cmdPool, modelPath, newMesh->m_material);
        }

        m_meshes.push_back(newMesh);
    }

    void Model::loadMaterialTextures(
        const aiMaterial* material,
        TextureLibrary& textureLib,
        const CommandPool& cmdPool,
        const std::filesystem::path& modelPath,
        std::shared_ptr<Material> vkMaterial) {

        aiString texturePath;

        // Diffuse texture
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
            auto fullPath = modelPath / texturePath.C_Str();
            vkMaterial->m_diffuseMap = textureLib.getOrLoadTexture(fullPath, cmdPool);
        }

        // Specular texture
        if (material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath) == AI_SUCCESS) {
            auto fullPath = modelPath / texturePath.C_Str();
            vkMaterial->m_specularMap = textureLib.getOrLoadTexture(fullPath, cmdPool);
        }

        // Normal texture 
        if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS ||
            material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == AI_SUCCESS) {
            auto fullPath = modelPath / texturePath.C_Str();
            vkMaterial->m_normalMap = textureLib.getOrLoadTexture(fullPath, cmdPool);
        }
    }

} // namespace vkcommon