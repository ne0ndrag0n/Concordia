#ifndef MOUSE_NAVIGATOR
#define MOUSE_NAVIGATOR

#include <glm/glm.hpp>

namespace BlueBear::Graphics{ class Camera; }
namespace BlueBear::Graphics::Utilities {

    class MouseNavigator {
        Camera& camera;
        glm::vec2 originalPoint;
        glm::vec2 updatedPoint;

    public:
        MouseNavigator( Camera& camera, const glm::vec2& originalPoint );

        void setVector( const glm::vec2& updatedPoint );
        void updateCamera();
    };

}


#endif