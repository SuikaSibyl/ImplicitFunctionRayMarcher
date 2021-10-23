#pragma once

#ifndef _COUNTERMODEL_
#define _COUNTERMODEL_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Model.h"
using namespace std;

struct Edge {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // tangent
    glm::vec3 Normal_1;
    // bitangent
    glm::vec3 Normal_2;

    Edge() { 
        Position = glm::vec3(0);
        Normal = glm::vec3(0);
        Normal_1 = glm::vec3(0);
        Normal_2 = glm::vec3(0);
    }
};

struct indEdge {
    // position
    unsigned int index_small;
    unsigned int index_big;

    indEdge(unsigned int a, unsigned int b)
    {
        if (a > b)
        {
            index_small = b;
            index_big = a;
        }
        else
        {
            index_small = a;
            index_big = b;
        }
    }

    bool operator<(const indEdge& l1) const {
        if (this->index_small < l1.index_small)
            return true;
        else if (this->index_small > l1.index_small)
            return false;
        else
        {
            if (this->index_big < l1.index_big)
                return true;
            else
                return false;
        }
    }
};

struct metaEdge {
    int neighbor[2];
    int n_neighbor = 0;

    void AddNeighbor(unsigned int i)
    {
        if (n_neighbor < 2)
        {
            neighbor[n_neighbor++] = i;
        }
        else
        {
            cout << "more than two face!";
        }
    }
};

class EdgeMesh
{
public:
    // mesh Data
    vector<Edge>        vertices;
    vector<unsigned int> indices;
    unsigned int VAO;
    // constructor
    EdgeMesh(vector<Edge> vertices, vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();

    }

    // render the mesh
    void Draw(SuikaGL::Shader& shader)
    {
        shader.use();
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Edge), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Edge), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Edge), (void*)offsetof(Edge, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Edge), (void*)offsetof(Edge, Normal_1));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Edge), (void*)offsetof(Edge, Normal_2));

        glBindVertexArray(0);
    }
};

class CounterModel:Model
{
public:
    vector<EdgeMesh> edgemeshes;

    bool doDrawCounter = true;

    void Draw(SuikaGL::Shader& shader, SuikaGL::Shader& edge_shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
        if (doDrawCounter)
        {
            for (unsigned int i = 0; i < edgemeshes.size(); i++)
                edgemeshes[i].Draw(edge_shader);
        }
    }

    // constructor, expects a filepath to a 3D model.
    CounterModel(string const& path, bool gamma = false) :Model(path, gamma)
    {
        loadModel(path);
    }

private:
    glm::vec3 getNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
        float x = (b.y - a.y) * (c.z - a.z) - (b.z - a.z) * (c.z - a.z);
        float y = (b.z - a.z) * (c.x - a.x) - (c.z - a.z) * (b.x - a.x);
        float z = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
        return glm::vec3(x, y, z);
    }

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            edgemeshes.push_back(processMesh(mesh, scene));

        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    EdgeMesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Edge> vertices;
        vector<unsigned int> indices;

        map<aiVector3D, unsigned int> m_posMap;
        map<indEdge, metaEdge> m_edgeMap;
        unsigned int faceIndices[3];

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];

            if (face.mNumIndices != 3)
            {
                cout << "Exist face with vertices number not 3 in model!";
                cout << (face.mNumIndices);
            }

            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                unsigned int index = face.mIndices[j];
                aiVector3D& v = mesh->mVertices[index]; 
                
                if (m_posMap.find(v) == m_posMap.end()) {
                    m_posMap[v] = index;
                }
                else {
                    index = m_posMap[v];
                }

                faceIndices[j] = index;
            }

            indEdge e1(faceIndices[0], faceIndices[1]);
            indEdge e2(faceIndices[1], faceIndices[2]);
            indEdge e3(faceIndices[2], faceIndices[0]);

            m_edgeMap[e1].AddNeighbor(i);
            m_edgeMap[e2].AddNeighbor(i);
            m_edgeMap[e3].AddNeighbor(i);
        }

        unsigned int global_index = 0;

        for (auto iter = m_edgeMap.begin(); iter != m_edgeMap.end(); iter++)
        {
            indEdge ind_edge = iter->first;
            metaEdge meta_edge = iter->second;

            aiFace& face = mesh->mFaces[meta_edge.neighbor[0]];
            if (!mesh->HasNormals())
            {
                cout << "No Normal information" << endl;
            }
            glm::vec3 pos_1;
            pos_1.x = mesh->mNormals[face.mIndices[0]].x;
            pos_1.y = mesh->mNormals[face.mIndices[0]].y;
            pos_1.z = mesh->mNormals[face.mIndices[0]].z;
            glm::vec3 pos_2;
            pos_2.x = mesh->mNormals[face.mIndices[1]].x;
            pos_2.y = mesh->mNormals[face.mIndices[1]].y;
            pos_2.z = mesh->mNormals[face.mIndices[1]].z;
            glm::vec3 pos_3;
            pos_3.x = mesh->mNormals[face.mIndices[2]].x;
            pos_3.y = mesh->mNormals[face.mIndices[2]].y;
            pos_3.z = mesh->mNormals[face.mIndices[2]].z;
            glm::vec3 normal_1 = glm::normalize(pos_1 + pos_2 + pos_3);
            if (normal_1.x == 0 && normal_1.y == 0)
            {
                cout << "strange" << endl;
                cout << normal_1.z << endl;
            }
            glm::vec3 normal_2 = glm::vec3(0);
            if (meta_edge.n_neighbor == 1)
            {
                normal_2 = -normal_1;
            }
            else if(meta_edge.n_neighbor == 2)
            {
                aiFace& face = mesh->mFaces[meta_edge.neighbor[1]];
                glm::vec3 pos_1;
                pos_1.x = mesh->mNormals[face.mIndices[0]].x;
                pos_1.y = mesh->mNormals[face.mIndices[0]].y;
                pos_1.z = mesh->mNormals[face.mIndices[0]].z;
                glm::vec3 pos_2;
                pos_2.x = mesh->mNormals[face.mIndices[1]].x;
                pos_2.y = mesh->mNormals[face.mIndices[1]].y;
                pos_2.z = mesh->mNormals[face.mIndices[1]].z;
                glm::vec3 pos_3;
                pos_3.x = mesh->mNormals[face.mIndices[2]].x;
                pos_3.y = mesh->mNormals[face.mIndices[2]].y;
                pos_3.z = mesh->mNormals[face.mIndices[2]].z;
                normal_2 = glm::normalize(pos_1 + pos_2 + pos_3);
            }

            unsigned int ind  = ind_edge.index_small;
            for (int i = 0; i < 2; i++)
            {
                if (i == 1) ind = ind_edge.index_big;
                Edge edge;

                glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

                // positions
                vector.x = mesh->mVertices[ind].x;
                vector.y = mesh->mVertices[ind].y;
                vector.z = mesh->mVertices[ind].z;
                edge.Position = vector;
                // normals
                if (mesh->HasNormals())
                {
                    vector.x = mesh->mNormals[ind].x;
                    vector.y = mesh->mNormals[ind].y;
                    vector.z = mesh->mNormals[ind].z;
                    edge.Normal = vector;
                }

                vertices.push_back(edge);

                edge.Normal_1 = normal_1;
                edge.Normal_2 = normal_2;

                vertices.push_back(edge);
            }

            indices.push_back(global_index + 0);
            indices.push_back(global_index + 1);
            indices.push_back(global_index + 2);

            indices.push_back(global_index + 1);
            indices.push_back(global_index + 2);
            indices.push_back(global_index + 3);

            global_index += 4;
        }

        //// walk through each of the mesh's vertices
        //for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        //{
        //    Vertex vertex;
        //    glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        //    // positions
        //    vector.x = mesh->mVertices[i].x;
        //    vector.y = mesh->mVertices[i].y;
        //    vector.z = mesh->mVertices[i].z;
        //    vertex.Position = vector;
        //    // normals
        //    if (mesh->HasNormals())
        //    {
        //        vector.x = mesh->mNormals[i].x;
        //        vector.y = mesh->mNormals[i].y;
        //        vector.z = mesh->mNormals[i].z;
        //        vertex.Normal = vector;
        //    }
        //    // texture coordinates
        //    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        //    {
        //        glm::vec2 vec;
        //        // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
        //        // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
        //        vec.x = mesh->mTextureCoords[0][i].x;
        //        vec.y = mesh->mTextureCoords[0][i].y;
        //        vertex.TexCoords = vec;
        //        // tangent
        //        vector.x = mesh->mTangents[i].x;
        //        vector.y = mesh->mTangents[i].y;
        //        vector.z = mesh->mTangents[i].z;
        //        vertex.Tangent = vector;
        //        // bitangent
        //        vector.x = mesh->mBitangents[i].x;
        //        vector.y = mesh->mBitangents[i].y;
        //        vector.z = mesh->mBitangents[i].z;
        //        vertex.Bitangent = vector;
        //    }
        //    else
        //        vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        //    vertices.push_back(vertex);
        //}
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        //for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        //{
        //    aiFace face = mesh->mFaces[i];
        //    // retrieve all indices of the face and store them in the indices vector
        //    for (unsigned int j = 0; j < face.mNumIndices; j++)
        //        indices.push_back(face.mIndices[j]);
        //}

        // return a mesh object created from the extracted mesh data
        return EdgeMesh(vertices, indices);
    }
};

#endif // !_COUNTERMODEL_
