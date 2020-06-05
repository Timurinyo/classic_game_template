//function lerp(a,b,t) return (1-t)*a + t*b

#include <engine/system.h>
#include <engine/external_libs.h>

glm::vec2 LerpVec2(glm::vec2 a, glm::vec2 b, float t)
{
    return (1.f - t) * a + t * b;
}