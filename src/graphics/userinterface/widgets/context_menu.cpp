#include "graphics/userinterface/widgets/context_menu.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/userinterface/style/style.hpp"
#include "containers/visitor.hpp"
#include "tools/utility.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

	ContextMenu::ContextMenu( const std::string& id, const std::vector< std::string >& classes, const std::vector< Entry >& items ) :
		Element::Element( "ContextMenu", id, classes ), items( items ) {}

	std::shared_ptr< ContextMenu > ContextMenu::create( const std::string& id, const std::vector< std::string >& classes, const std::vector< Entry >& items ) {
		std::shared_ptr< ContextMenu > contextMenu( new ContextMenu( id, classes, items ) );

    	return contextMenu;
	}

	std::vector< ContextMenu::Entry > ContextMenu::parseEntries( const std::string& text ) {
		/*
			Format in XML:
			id:label\n
			id:label\n
			---\n
			id:label\n
		*/

		std::vector< ContextMenu::Entry > result;

		std::vector< std::string > lines = Tools::Utility::split( Tools::Utility::stringTrim( text ), '\n' );
		for( auto line : lines ) {
			line = Tools::Utility::stringTrim( line );

			if( line == "---" ) {
				result.emplace_back( Divider() );
			} else {
				std::vector< std::string > pair = Tools::Utility::split( text, ':' );
				if( pair.size() == 2 ) {
					result.emplace_back( Item{ pair[ 0 ], pair[ 1 ] } );
				}
			}
		}

		return result;
	}

	std::string ContextMenu::getLongestLabel() const {
		std::pair< std::string, int > sizeData = { "", 0 };

		for( const auto& item : items ) {
			std::visit( overloaded {
				[ &sizeData ]( const Item& item ) {
					if( item.label.length() > sizeData.second ) {
						sizeData.first = item.label;
						sizeData.second = item.label.length();
					}
				},
				[]( const Divider& divider ) { /* Nothing */ }
			}, item );
		}

		return sizeData.first;
	}

	void ContextMenu::calculate() {
		int padding = localStyle.get< int >( "padding" );
		double fontSize = localStyle.get< double >( "font-size" );
		glm::vec4 size = manager->getVectorRenderer().getTextSizeParams( localStyle.get< std::string >( "font" ), getLongestLabel(), fontSize );
    	longestTextSpan = size[ 2 ];
		textHeight = size[ 3 ];

		requisition = glm::uvec2{
			( padding * 2 ) + longestTextSpan + 10,
			( padding * 2 ) + ( items.size() * textHeight )
		};
	}
}