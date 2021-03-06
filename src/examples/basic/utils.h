//function lerp(a,b,t) return (1-t)*a + t*b

#include <engine/system.h>
#include <engine/external_libs.h>

glm::vec2 LerpVec2(glm::vec2 a, glm::vec2 b, float t)
{
    t = t > 1 ? 1.0 : t;

    glm::vec2 result = glm::vec2((1.f - t) * a.x + t * b.x, (1.f - t) * a.y + t * b.y);

    return result;
}

float LerpFloat(float a, float b, float t)
{    
    t = t > 1 ? 1.0 : t;

    return (1.f - t) * a + t * b;
}