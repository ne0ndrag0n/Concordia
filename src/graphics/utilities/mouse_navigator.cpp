#include "graphics/utilities/mouse_navigator.hpp"
#include "graphics/camera.hpp"

#include "log.hpp"
#include <glm/gtx/string_cast.hpp>

namespace BlueBear::Graphics::Utilities {

    MouseNavigator::MouseNavigator( Camera& camera, const glm::vec2& originalPoint ) :
        camera( camera ), originalPoint( originalPoint ), updatedPoint( originalPoint ) {}

    void MouseNavigator::setVector( const glm::vec2& updatedPoint ) {
        this->updatedPoint = updatedPoint;
    }

    void MouseNavigator::updateCamera() {
        glm::vec2 direction = glm::sign( updatedPoint - originalPoint ) * glm::vec2( -1.0f, 1.0f );
        float magnitude = std::floor( std::abs( glm::distance( originalPoint, updatedPoint ) ) / 10 );

        glm::vec3 cameraPosition = camera.getPosition();
        cameraPosition += glm::vec3( direction, 0.0f ) * magnitude;
        camera.setPosition( cameraPosition );
    }

}