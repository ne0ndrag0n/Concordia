#include "graphics/utilities/mouse_navigator.hpp"
#include "graphics/camera.hpp"
#include "tools/utility.hpp"

namespace BlueBear::Graphics::Utilities {

    MouseNavigator::MouseNavigator( Camera& camera, const glm::vec2& originalPoint ) :
        camera( camera ), originalPoint( originalPoint ), updatedPoint( originalPoint ) {}

    void MouseNavigator::setVector( const glm::vec2& updatedPoint ) {
        this->updatedPoint = updatedPoint;
    }

    glm::vec2 MouseNavigator::getDirection() const {
        glm::vec2 direction = glm::sign( updatedPoint - originalPoint ) * glm::vec2( -1.0f, 1.0f );

        // Base line for angles starts at original point and extends right
        // Angle is either 0-22.5, 157.5-202.5, or 337.5-360: Tolerate X and clear Y param
        // Angle is either 67.5-112.5 or 247.5-292.5: Tolerate Y and clear X param
        // Anything else - leave it!
        float angle = Tools::Utility::positiveAngle(
            glm::degrees(
                std::atan2( updatedPoint.y - originalPoint.y, updatedPoint.x - originalPoint.x )
            )
        );

        if(
            Tools::Utility::rangeInclusive( angle, 0.0f, 22.5f ) ||
            Tools::Utility::rangeInclusive( angle, 157.5f, 202.5f ) ||
            Tools::Utility::rangeInclusive( angle, 337.5f, 360.0f )
        ) {
            direction.y = 0.0f;
        }

        if(
            Tools::Utility::rangeInclusive( angle, 67.5f, 112.5f ) ||
            Tools::Utility::rangeInclusive( angle, 247.5f, 292.5f )
        ) {
            direction.x = 0.0f;
        }

        return direction;
    }

    void MouseNavigator::updateCamera() {
        glm::vec2 direction = getDirection();
        float magnitude = std::floor( std::abs( glm::distance( originalPoint, updatedPoint ) ) / 10 );

        glm::vec3 cameraPosition = camera.getPosition();
        cameraPosition += glm::vec3( direction, 0.0f ) * magnitude;
        camera.setPosition( cameraPosition );
    }

}