#include <vector>
#include "2dobject.h"
#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
       
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill)
    {
        const int numSegments = 100;
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        
        vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), color);

       
        for (int i = 0; i <= numSegments; ++i)
        {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            vertices.emplace_back(glm::vec3(x, y, 0.0f), color);
        }

        Mesh* circle = new Mesh(name);

        
        for (int i = 1; i <= numSegments; ++i)
        {
            indices.push_back(0);    
            indices.push_back(i);   
            indices.push_back(i % numSegments + 1); 
        }

        circle->InitFromData(vertices, indices);

        if (fill) {
            circle->SetDrawMode(GL_TRIANGLES);
        } else {
            circle->SetDrawMode(GL_LINE_LOOP);
        }

        return circle;
    }

    Mesh* object2D::CreateTrapezoid(
        const std::string &name,
        glm::vec3 leftBottomCorner,
        float bottomWidth,
        float topWidth,
        float height,
        glm::vec3 color,
        bool fill)
    {
        glm::vec3 corner = leftBottomCorner;

        float halfDifference = (bottomWidth - topWidth) / 2.0f;

        std::vector<VertexFormat> vertices =
        {
            VertexFormat(corner, color),
            VertexFormat(corner + glm::vec3(bottomWidth, 0, 0), color), 
            VertexFormat(corner + glm::vec3(bottomWidth - halfDifference, height, 0), color), 
            VertexFormat(corner + glm::vec3(halfDifference, height, 0), color) 
        };

        Mesh* trapezoid = new Mesh(name);
        std::vector<unsigned int> indices = { 0, 1, 2, 3 };

        if (!fill) {
            trapezoid->SetDrawMode(GL_LINE_LOOP);
        } else {
            
            indices.push_back(0);
            indices.push_back(2);
        }

        trapezoid->InitFromData(vertices, indices);
        return trapezoid;
    }


