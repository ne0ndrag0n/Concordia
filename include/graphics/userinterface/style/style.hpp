#ifndef STYLE_ELEMENT
#define STYLE_ELEMENT

#include "exceptions/genexc.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <string>
#include <memory>
#include <type_traits>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        class Style {
        public:
          class Animation {
          public:
            struct Keyframe {
              PropertyList properties;
              bool interpolate = false;
            };

          private:
            Style* parent;
            std::map< double, Keyframe > keyframes;
            const double fps;
            const double duration;
            double current;
            const bool suicide;

            double getFPS();

          public:
            EXCEPTION_TYPE( MalformedKeyframesException, "Keyframes malformed" );

            Animation( Style* parent, std::map< double, Keyframe > keyframes, double fps, double duration, bool suicide );

            void increment();

            template < typename VariantType > const VariantType getKeyed( const std::string& key ) const {

              // If the type is in a keyframe directly, no interpolation has to be done
              // Return it directly
              auto direct = keyframes.find( current );
              if( direct != keyframes.end() && direct->second.properties.keyExists( key ) ) {
                return direct->second.properties.get< VariantType >( key );
              }

              // Now we have to get the one leftmost first
              VariantType left;
              double leftTick;
              auto leftmost = keyframes.lower_bound( current );
              if( leftmost != keyframes.end() && leftmost->second.properties.keyExists( key ) ) {
                left = leftmost->second.properties.get< VariantType >( key );
                leftTick = leftmost->first;
              } else {
                left = parent->get< VariantType >( key );
                leftTick = 0.0;
              }

              if constexpr( std::is_same< VariantType, int >::value || std::is_same< VariantType, double >::value || std::is_same< VariantType, glm::uvec4 >::value ) {
                // Value is interpolatable
                auto rightmost = leftmost;
                std::advance( rightmost, 1 );

                for( ; rightmost != keyframes.end(); ++rightmost ) {
                  if( rightmost->second.properties.keyExists( key ) ) {
                    break;
                  }
                }

                if( rightmost == keyframes.end() || !rightmost->second.interpolate ) {
                  // Nothing to the right, or it is there but we don't need it
                  return left;
                }

                // Interpolate between the two
                if constexpr( std::is_same< VariantType, glm::uvec4 >::value ) {
                  return glm::mix(
                    left,
                    rightmost->second.properties.get< VariantType >( key ),
                    ( current - leftTick ) / ( rightmost->first - leftTick )
                  );
                } else {
                  return Tools::Utility::interpolateLinear(
                    left,
                    rightmost->second.properties.get< VariantType >( key ),
                    ( current - leftTick ) / ( rightmost->first - leftTick )
                  );
                }

              } else {
                // Value cannot be interpolated so don't even bother looking right
                return left;
              }
            };
          };

        private:
          Element* parent;
          PropertyList calculated;
          PropertyList local;
          std::unordered_set< std::string > changedAttributes;
          std::unique_ptr< Animation > attachedAnimation;

        public:
          Style( Element* parent );
          ~Style();

          const std::unordered_set< std::string >& getChangedAttributes();
          void resetChangedAttributes();
          void reflowParent();
          void setCalculated( const std::unordered_map< std::string, std::any >& map );
          void resetProperty( const std::string& key );

          void attachAnimation( std::unique_ptr< Animation > animation );
          void updateAnimation();

          template < typename VariantType > const VariantType inheritedGet( const std::string& key ) const {
            if( local.keyExists( key ) ) {
              return local.get< VariantType >( key );
            } else if( calculated.keyExists( key ) ) {
              return calculated.get< VariantType >( key );
            } else {
              return PropertyList::rootPropertyList.get< VariantType >( key );
            }
          };

          template < typename VariantType > const VariantType get( const std::string& key ) const {
            if( attachedAnimation ) {
              return attachedAnimation->getKeyed< VariantType >( key );
            } else {
              return inheritedGet< VariantType >( key );
            }
          };

          template < typename VariantType > void set( const std::string& key, VariantType value, bool reflow = true ) {
            local.set< VariantType >( key, value );
            changedAttributes.insert( key );

            if( reflow ) {
              reflowParent();
            }
          };
        };

      }
    }
  }
}

#endif
