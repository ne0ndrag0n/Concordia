#ifndef TRANSFORMEDPATHIMAGEBUILDER
#define TRANSFORMEDPATHIMAGEBUILDER

#include "graphics/imagebuilder/pathimagebuilder.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

namespace BlueBear {
  namespace Graphics {

    class TransformedPathImageBuilder : public PathImageBuilder {
      public:
        sf::Image getImage();
        std::string getKey();

        class Transform {
          public:
            virtual sf::Image transform( sf::Image& image ) = 0;
            virtual std::string getKeyComponent() = 0;
        };


        /**
         * Crop the image to a part of the original
         */
        class CropTransform : public Transform {
          int x,y,w,h;

          public:
            CropTransform( int x, int y, int w, int h );
            sf::Image transform( sf::Image& image );
            std::string getKeyComponent();
        };

        TransformedPathImageBuilder( const std::string& path );
        TransformedPathImageBuilder( const std::string& path, std::unique_ptr< Transform > transform );

        void applyTransform( std::unique_ptr< Transform > transform );
        void clearTransforms();

        private:
          std::vector< std::unique_ptr< Transform > > transforms;
    };

  }
}


#endif
