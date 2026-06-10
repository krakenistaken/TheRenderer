#include "OBJLoader.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <array>
#include <iomanip>
#include <filesystem>
#include <cstdio>
#include <cmath>

namespace OBJLoader {

struct VertexKey {
    int v, vn, vt;
    bool operator==(const VertexKey& other) const {
        return v == other.v && vn == other.vn && vt == other.vt;
    }
};

struct VertexKeyHash {
    size_t operator()(const VertexKey& k) const {
        size_t h = std::hash<int>()(k.v);
        h ^= std::hash<int>()(k.vn) << 1;
        h ^= std::hash<int>()(k.vt) << 2;
        return h;
    }
};

bool load(const std::string& filepath, std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        fprintf(stderr, "Failed to open OBJ file: %s\n", filepath.c_str());
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::unordered_map<VertexKey, uint32_t, VertexKeyHash> vertexMap;

    outVertices.clear();
    outIndices.clear();

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        } else if (prefix == "vn") {
            glm::vec3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        } else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        } else if (prefix == "f") {
            std::vector<uint32_t> faceIndices;
            std::string token;
            while (iss >> token) {
                VertexKey key = {0, 0, 0};
                size_t pos1 = token.find('/');
                if (pos1 == std::string::npos) {
                    key.v = std::stoi(token);
                } else {
                    key.v = std::stoi(token.substr(0, pos1));
                    size_t pos2 = token.find('/', pos1 + 1);
                    if (pos2 == std::string::npos) {
                        if (pos1 + 1 < token.size())
                            key.vt = std::stoi(token.substr(pos1 + 1));
                    } else {
                        if (pos2 > pos1 + 1)
                            key.vt = std::stoi(token.substr(pos1 + 1, pos2 - pos1 - 1));
                        if (pos2 + 1 < token.size())
                            key.vn = std::stoi(token.substr(pos2 + 1));
                    }
                }

                auto it = vertexMap.find(key);
                if (it != vertexMap.end()) {
                    faceIndices.push_back(it->second);
                } else {
                    Vertex vert = {};
                    if (key.v > 0 && key.v <= (int)positions.size())
                        vert.position = positions[key.v - 1];
                    if (key.vn > 0 && key.vn <= (int)normals.size())
                        vert.normal = normals[key.vn - 1];
                    if (key.vt > 0 && key.vt <= (int)texcoords.size())
                        vert.texCoords = texcoords[key.vt - 1];

                    uint32_t idx = static_cast<uint32_t>(outVertices.size());
                    outVertices.push_back(vert);
                    vertexMap[key] = idx;
                    faceIndices.push_back(idx);
                }
            }

            // Fan triangulation for convex polygons
            for (size_t i = 1; i + 1 < faceIndices.size(); i++) {
                outIndices.push_back(faceIndices[0]);
                outIndices.push_back(faceIndices[i]);
                outIndices.push_back(faceIndices[i + 1]);
            }
        }
    }

    // Compute normals if none were provided
    if (normals.empty() && !outVertices.empty()) {
        for (auto& v : outVertices) v.normal = glm::vec3(0.0f);
        for (size_t i = 0; i + 2 < outIndices.size(); i += 3) {
            const auto& v0 = outVertices[outIndices[i]].position;
            const auto& v1 = outVertices[outIndices[i+1]].position;
            const auto& v2 = outVertices[outIndices[i+2]].position;
            glm::vec3 n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            outVertices[outIndices[i]].normal += n;
            outVertices[outIndices[i+1]].normal += n;
            outVertices[outIndices[i+2]].normal += n;
        }
        for (auto& v : outVertices) {
            if (glm::length(v.normal) > 0.0001f)
                v.normal = glm::normalize(v.normal);
        }
    }

    printf("Loaded OBJ: %zu vertices, %zu triangles\n", outVertices.size(), outIndices.size() / 3);
    return true;
}

void generateIcosphere(const std::string& filepath, int subdivisions) {
    // Check if file already exists
    std::ifstream test(filepath);
    if (test.good()) { test.close(); return; }
    test.close();

    float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

    std::vector<glm::vec3> verts;
    auto addVert = [&](float x, float y, float z) -> int {
        verts.push_back(glm::normalize(glm::vec3(x, y, z)));
        return (int)verts.size() - 1;
    };

    // Icosahedron vertices
    addVert(-1,  t,  0); addVert( 1,  t,  0); addVert(-1, -t,  0); addVert( 1, -t,  0);
    addVert( 0, -1,  t); addVert( 0,  1,  t); addVert( 0, -1, -t); addVert( 0,  1, -t);
    addVert( t,  0, -1); addVert( t,  0,  1); addVert(-t,  0, -1); addVert(-t,  0,  1);

    std::vector<std::array<int, 3>> faces = {
        {0,11,5}, {0,5,1}, {0,1,7}, {0,7,10}, {0,10,11},
        {1,5,9}, {5,11,4}, {11,10,2}, {10,7,6}, {7,1,8},
        {3,9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9},
        {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1}
    };

    // Subdivide
    for (int s = 0; s < subdivisions; s++) {
        std::map<std::pair<int,int>, int> midCache;
        auto getMid = [&](int i1, int i2) -> int {
            auto key = std::make_pair(std::min(i1,i2), std::max(i1,i2));
            auto it = midCache.find(key);
            if (it != midCache.end()) return it->second;
            glm::vec3 mid = glm::normalize((verts[i1] + verts[i2]) * 0.5f);
            verts.push_back(mid);
            int idx = (int)verts.size() - 1;
            midCache[key] = idx;
            return idx;
        };

        std::vector<std::array<int,3>> newFaces;
        newFaces.reserve(faces.size() * 4);
        for (const auto& f : faces) {
            int a = f[0], b = f[1], c = f[2];
            int ab = getMid(a, b);
            int bc = getMid(b, c);
            int ca = getMid(c, a);
            newFaces.push_back({a, ab, ca});
            newFaces.push_back({b, bc, ab});
            newFaces.push_back({c, ca, bc});
            newFaces.push_back({ab, bc, ca});
        }
        faces = std::move(newFaces);
    }

    // Create directory if needed
    auto parentPath = std::filesystem::path(filepath).parent_path();
    if (!parentPath.empty())
        std::filesystem::create_directories(parentPath);

    // Write OBJ
    std::ofstream out(filepath);
    if (!out.is_open()) {
        fprintf(stderr, "Failed to create OBJ file: %s\n", filepath.c_str());
        return;
    }

    out << "# Icosphere (" << subdivisions << " subdivisions)\n";
    out << "# " << verts.size() << " vertices, " << faces.size() << " faces\n\n";

    out << std::fixed << std::setprecision(6);
    for (const auto& v : verts)
        out << "v " << v.x << " " << v.y << " " << v.z << "\n";
    out << "\n";

    // Normals = positions (unit sphere)
    for (const auto& v : verts)
        out << "vn " << v.x << " " << v.y << " " << v.z << "\n";
    out << "\n";

    for (const auto& f : faces) {
        int a = f[0]+1, b = f[1]+1, c = f[2]+1;
        out << "f " << a << "//" << a << " " << b << "//" << b << " " << c << "//" << c << "\n";
    }

    printf("Generated icosphere: %zu vertices, %zu faces -> %s\n", verts.size(), faces.size(), filepath.c_str());
}

void generateTorus(const std::string& filepath, int mainSegments, int tubeSegments, float mainRadius, float tubeRadius) {
    std::ifstream test(filepath);
    if (test.good()) { test.close(); return; }
    test.close();

    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> uvs;
    std::vector<std::array<int, 3>> faces;

    float mainStep = 2.0f * 3.14159265359f / mainSegments;
    float tubeStep = 2.0f * 3.14159265359f / tubeSegments;

    for (int i = 0; i < mainSegments; i++) {
        float u = i * mainStep;
        float cosU = std::cos(u);
        float sinU = std::sin(u);

        for (int j = 0; j < tubeSegments; j++) {
            float v = j * tubeStep;
            float cosV = std::cos(v);
            float sinV = std::sin(v);

            float x = (mainRadius + tubeRadius * cosV) * cosU;
            float y = tubeRadius * sinV;
            float z = (mainRadius + tubeRadius * cosV) * sinU;

            float nx = cosV * cosU;
            float ny = sinV;
            float nz = cosV * sinU;

            float u_coord = (float)i / mainSegments;
            float v_coord = (float)j / tubeSegments;

            verts.push_back(glm::vec3(x, y, z));
            norms.push_back(glm::normalize(glm::vec3(nx, ny, nz)));
            uvs.push_back(glm::vec2(u_coord, v_coord));
        }
    }

    for (int i = 0; i < mainSegments; i++) {
        for (int j = 0; j < tubeSegments; j++) {
            int nextI = (i + 1) % mainSegments;
            int nextJ = (j + 1) % tubeSegments;

            int a = i * tubeSegments + j;
            int b = nextI * tubeSegments + j;
            int c = nextI * tubeSegments + nextJ;
            int d = i * tubeSegments + nextJ;

            // Two triangles for each quad
            faces.push_back({a, b, c});
            faces.push_back({a, c, d});
        }
    }

    auto parentPath = std::filesystem::path(filepath).parent_path();
    if (!parentPath.empty()) std::filesystem::create_directories(parentPath);

    std::ofstream out(filepath);
    if (!out.is_open()) {
        fprintf(stderr, "Failed to create OBJ file: %s\n", filepath.c_str());
        return;
    }

    out << "# Torus\n";
    out << std::fixed << std::setprecision(6);
    
    for (const auto& v : verts) out << "v " << v.x << " " << v.y << " " << v.z << "\n";
    out << "\n";
    for (const auto& n : norms) out << "vn " << n.x << " " << n.y << " " << n.z << "\n";
    out << "\n";
    for (const auto& uv : uvs) out << "vt " << uv.x << " " << uv.y << "\n";
    out << "\n";

    for (const auto& f : faces) {
        int a = f[0]+1, b = f[1]+1, c = f[2]+1;
        out << "f " << a << "/" << a << "/" << a << " " 
                    << b << "/" << b << "/" << b << " " 
                    << c << "/" << c << "/" << c << "\n";
    }

    printf("Generated torus: %zu vertices, %zu faces -> %s\n", verts.size(), faces.size(), filepath.c_str());
}

} // namespace OBJLoader
