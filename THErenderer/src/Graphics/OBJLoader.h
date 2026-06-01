#pragma once
#include <string>
#include <vector>
#include "Mesh.h"

namespace OBJLoader {
    bool load(const std::string& filepath, std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices);
    void generateIcosphere(const std::string& filepath, int subdivisions = 3);
    void generateTorus(const std::string& filepath, int mainSegments = 48, int tubeSegments = 24, float mainRadius = 1.0f, float tubeRadius = 0.4f);
}
