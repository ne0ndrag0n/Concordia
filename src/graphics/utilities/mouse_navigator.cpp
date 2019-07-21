#include "graphics/utilities/mouse_navigator.hpp"
#include "graphics/camera.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"

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

        float scrollSnap = ConfigManager::getInstance().getIntValue( "camera_scroll_snap" );
        float scrollSnapHalf = scrollSnap * 0.5f;

        if(
            Tools::Utility::rangeInclusive( angle, 0.0f, scrollSnapHalf ) ||
            Tools::Utility::rangeInclusive( angle, 180.0f - scrollSnapHalf, 180.0f + scrollSnapHalf ) ||
            Tools::Utility::rangeInclusive( angle, 360.0f - scrollSnapHalf, 360.0f )
        ) {
            direction.y = 0.0f;
        }

        if(
            Tools::Utility::rangeInclusive( angle, 90.0f - scrollSnapHalf, 90.0f + scrollSnapHalf ) ||
            Tools::Utility::rangeInclusive( angle, 270.0f - scrollSnapHalf, 270.0f + scrollSnapHalf )
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