#include <iostream>
#include <fstream>
#include <stdio.h>

#include "Mesh.h"

Mesh::Mesh() {
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
    vertexCount = 0;
}

void outputMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices) {
    char buf[1024];
    std::ofstream meshFile;
    meshFile.open("D:\\tmp\\mesh_cpp.txt");
    meshFile << "Vertices...\n";
    GLfloat* vtxPtr = vertices;
    for (unsigned int i = 0; i < numOfVertices; i += 8) {
        //sprintf_s(buf, "%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f\n", *vtxPtr, *(vtxPtr + 1), *(vtxPtr + 2), *(vtxPtr + 3), *(vtxPtr + 4), *(vtxPtr + 5), *(vtxPtr + 6), *(vtxPtr + 7));
        meshFile << buf;
        vtxPtr += 8;
    }
    meshFile << "Indices...\n";
    unsigned int* idxPtr = indices;
    for (unsigned int i = 0; i < numOfIndices; i += 3) {
        //sprintf_s(buf, "%d, %d, %d\n", *idxPtr, *(idxPtr + 1), *(idxPtr + 2));
        meshFile << buf;
        idxPtr += 3;
    }
    meshFile.close();
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices) {
	indexCount = numOfIndices;
    vertexCount = numOfVertices;
    //printf("Mesh has %d indices\n", numOfIndices);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    outputMesh(vertices, indices, numOfVertices, numOfIndices);
}

void Mesh::RenderMesh() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::ClearMesh() {
    if (IBO != 0) {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}

GLsizei Mesh::getIndexCount() {
    return indexCount;
}

GLsizei Mesh::getVertexCount() {
    return vertexCount;
}

Mesh::~Mesh() {
    ClearMesh();
}