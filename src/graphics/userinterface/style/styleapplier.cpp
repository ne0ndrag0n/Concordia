#include "graphics/userinterface/style/styleapplier.hpp"
#include "graphics/userinterface/style/parser.hpp"
#include "log.hpp"
#include <fstream>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        StyleApplier::StyleApplier( std::shared_ptr< Element > rootElement ) : rootElement( rootElement ) {}

        void StyleApplier::associatePropertyList( const AST::PropertyList& propertyList ) {

        }

        void StyleApplier::applyStyles( std::vector< std::string > paths ) {
          // * Load from file using an individual Style::Parser
          // * Find elements it applies to using specificity rules
          // * Write element rules (reflow should be the responsibility of the call that sets the style)

          for( const std::string& path : paths ) {
            std::vector< AST::PropertyList > stylesheet;

            try {
              Parser parser( path );
              stylesheet = parser.getStylesheet();
            } catch( std::exception e ) {
              Log::getInstance().warn( "StyleApplier::applyStyles", "Failed to load .style file: " + path + " (" + e.what() + ")" );
              continue;
            }

            for( const AST::PropertyList& propertyList : stylesheet ) {
              associatePropertyList( propertyList );
            }
          }
        }

      }
    }
  }
}
