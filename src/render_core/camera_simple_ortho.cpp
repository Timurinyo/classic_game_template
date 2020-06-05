#include <render_core/pch.h>

#include <render_core/camera_simple_ortho.h>

namespace cgt::render
{

glm::mat4 CameraSimpleOrtho::GetViewProjection() const
{
    const float unitsPerPixel = 1.0f / pixelsPerUnit;

    const glm::vec2 centerOffset = unitsPerPixel * glm::vec2(
        (float)(windowWidth / 2),
        (float)(windowHeight / 2));

    glm::vec2 alignedPosition = position - centerOffset;

    alignedPosition.x = alignedPosition.x - std::fmodf(alignedPosition.x, unitsPerPixel);
    alignedPosition.y = alignedPosition.y - std::fmodf(alignedPosition.y, unitsPerPixel);

    const glm::mat4 view = glm::lookAt(
        glm::vec3(alignedPosition, -1.0f),
        glm::vec3(alignedPosition, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::mat4 projection = glm::ortho(
        0.0f,
        windowWidth * unitsPerPixel,
        0.0f,
        windowHeight * unitsPerPixel,
        0.0f,
        1.0f);

    return projection * view;
}

}
