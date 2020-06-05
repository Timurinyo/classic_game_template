#pragma once

#include <render_core/i_camera.h>

namespace cgt::render
{

class CameraSimpleOrtho : public ICamera
{
public:
    glm::mat4 GetViewProjection() const override;

    u32 windowWidth;
    u32 windowHeight;

    float pixelsPerUnit;

    glm::vec2 position = glm::vec2(0.0f);
};

}
