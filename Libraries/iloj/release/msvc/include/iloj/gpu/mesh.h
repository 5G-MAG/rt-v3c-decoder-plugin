/*
* Copyright (c) 2024 InterDigital R&D France
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#pragma once

#include "drawable.h"

namespace iloj::gpu
{
class Mesh
{
public:
    struct Attribute
    {
        enum
        {
            None = 0,
            Vertex = 1,
            TexCoord = 2,
            Color = 4,
            Normal = 8,
            All = 15
        };
    };
    struct SideMode
    {
        enum
        {
            None = 0,
            Front = 1,
            Back = 2,
            All = 3
        };
    };

private:
    unsigned m_faceMode = 0;
    std::vector<unsigned> m_indices;
    unsigned m_indexMode = 0;
    std::vector<Vec3f> m_vertices;
    unsigned m_vertexMode = 0;
    std::vector<Vec3f> m_texCoords;
    unsigned m_texCoordMode = 0;
    std::vector<GLColor> m_colors;
    unsigned m_colorMode = 0;
    std::vector<Vec3f> m_normals;
    unsigned m_normalMode = 0;

public:
    void setFaceMode(unsigned mode) { m_faceMode = mode; }
    [[nodiscard]] auto getFaceMode() const -> unsigned { return m_faceMode; }
    [[nodiscard]] auto getVerticesPerFace() const -> unsigned;
    void setIndices(std::vector<unsigned> indices, unsigned mode = 0)
    {
        m_indices = std::move(indices);
        m_indexMode = mode;
    }
    auto getIndices() -> std::vector<unsigned> & { return m_indices; }
    [[nodiscard]] auto getIndices() const -> const std::vector<unsigned> & { return m_indices; }
    void setIndexMode(unsigned mode) { m_indexMode = mode; }
    [[nodiscard]] auto getIndexMode() const -> unsigned { return m_indexMode; }
    void setVertices(std::vector<Vec3f> vertices, unsigned mode = 0)
    {
        m_vertices = std::move(vertices);
        m_vertexMode = mode;
    }
    [[nodiscard]] auto hasVertices() const -> bool { return !m_vertices.empty(); }
    auto getVertices() -> std::vector<Vec3f> & { return m_vertices; }
    [[nodiscard]] auto getVertices() const -> const std::vector<Vec3f> & { return m_vertices; }
    void setVertexMode(unsigned mode) { m_vertexMode = mode; }
    [[nodiscard]] auto getVertexMode() const -> unsigned { return m_vertexMode; }
    void setTextureCoordinates(std::vector<Vec3f> texCoords, unsigned mode = 0)
    {
        m_texCoords = std::move(texCoords);
        m_texCoordMode = mode;
    }
    [[nodiscard]] auto hasTextureCoordinates() const -> bool { return !m_texCoords.empty(); }
    auto getTextureCoordinates() -> std::vector<Vec3f> & { return m_texCoords; }
    [[nodiscard]] auto getTextureCoordinates() const -> const std::vector<Vec3f> & { return m_texCoords; }
    void setTextureCoordinateMode(unsigned mode) { m_texCoordMode = mode; }
    [[nodiscard]] auto getTextureCoordinateMode() const -> unsigned { return m_texCoordMode; }
    void setColors(std::vector<GLColor> colors, unsigned mode = 0)
    {
        m_colors = std::move(colors);
        m_colorMode = mode;
    }
    [[nodiscard]] auto hasColors() const -> bool { return !m_colors.empty(); }
    auto getColors() -> std::vector<GLColor> & { return m_colors; }
    [[nodiscard]] auto getColors() const -> const std::vector<GLColor> & { return m_colors; }
    void setColorMode(unsigned mode) { m_colorMode = mode; }
    [[nodiscard]] auto getColorMode() const -> unsigned { return m_colorMode; }
    void setNormals(std::vector<Vec3f> normals, unsigned mode = 0)
    {
        m_normals = std::move(normals);
        m_normalMode = mode;
    }
    [[nodiscard]] auto hasNormals() const -> bool { return !m_normals.empty(); }
    auto getNormals() -> std::vector<Vec3f> & { return m_normals; }
    [[nodiscard]] auto getNormals() const -> const std::vector<Vec3f> & { return m_normals; }
    void setNormalMode(unsigned mode) { m_normalMode = mode; }
    [[nodiscard]] auto getNormalMode() const -> unsigned { return m_normalMode; }
    [[nodiscard]] auto getBoundingBox() const -> std::pair<Vec3f, Vec3f>;
    [[nodiscard]] auto toDrawable() const -> Drawable;

public:
    static auto Quad() -> Mesh;
    static auto PointIndex(unsigned max_points) -> Mesh;
    static auto PointCloud(unsigned max_points, bool color) -> Mesh;
    static auto PointGrid(unsigned w, unsigned h) -> Mesh;
    static auto DiamondGrid(unsigned w, unsigned h) -> Mesh;
    static auto TriangleGrid(unsigned w, unsigned h) -> Mesh;
    static auto Patch() -> Mesh;
    static auto PatchGrid(unsigned w, unsigned h) -> Mesh;
    static auto Cube(bool colored = true) -> Mesh;
    static auto CubeGrid(unsigned nx, unsigned ny, unsigned nz) -> Mesh;
    static auto Icosahedron(bool colored = true) -> Mesh;
};
} // namespace iloj::gpu
