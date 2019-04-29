#include "graphics/userinterface/widgets/context_menu.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/userinterface/style/style.hpp"
#include "graphics/vector/renderer.hpp"
#include "scripting/luakit/utility.hpp"
#include "containers/visitor.hpp"
#include "tools/utility.hpp"
#include <glm/glm.hpp>
#include "log.hpp"

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

		Log::getInstance().debug( "Got", "Here" );

		std::vector< ContextMenu::Entry > result;

		std::vector< std::string > lines = Tools::Utility::split( Tools::Utility::stringTrim( text ), '\n' );
		for( auto line : lines ) {
			line = Tools::Utility::stringTrim( line );

			if( line == "---" ) {
				result.emplace_back( Divider() );
			} else {
				std::vector< std::string > pair = Tools::Utility::split( line, ':' );
				if( pair.size() == 2 ) {
					result.emplace_back( Item{ pair[ 0 ], pair[ 1 ] } );
				}
			}
		}

		return result;
	}

	std::vector< ContextMenu::Entry > ContextMenu::parseTable( sol::table table ) {
		std::vector< ContextMenu::Entry > result;

		for( auto& pair : table ) {
			if( Scripting::LuaKit::Utility::cast< std::string >( pair.second ) == "---" ) {
				result.emplace_back( Divider() );
			} else {
				result.emplace_back( Item{
					Scripting::LuaKit::Utility::cast< std::string >( pair.first ),
					Scripting::LuaKit::Utility::cast< std::string >( pair.second )
				} );
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

	void ContextMenu::render( Vector::Renderer& renderer ) {
		glm::ivec2 origin = { 5, 5 };
		glm::ivec2 dimensions = { allocation[ 2 ] - 5, allocation[ 3 ] - 5 };

		// Drop shadow
		// Left segment
		renderer.drawLinearGradient(
			{ 0, origin.y, origin.x, dimensions.y },
			{ origin.x, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ), 0, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ) },
			{ 0, 0, 0, 128 },
			{ 0, 0, 0, 0 }
		);
		// Top left corner
		renderer.drawScissored( { 0, 0, origin.x, origin.y }, [ & ]() {
			renderer.drawRadialGradient( origin, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
		} );
		// Top segment
		renderer.drawLinearGradient(
			{ 5, 0, dimensions.x, 5 },
			{ ( origin.x + ( ( dimensions.x - origin.x ) / 2 ) ), 5, ( origin.x + ( ( dimensions.x - origin.x ) / 2 ) ), 0 },
			{ 0, 0, 0, 128 },
			{ 0, 0, 0, 0 }
		);
		// Top right corner
		renderer.drawScissored( { allocation[ 2 ] - origin.x, 0, allocation[ 2 ], origin.y }, [ & ]() {
			renderer.drawRadialGradient( { allocation[ 2 ] - origin.x, origin.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
		} );
		// Right segment
		renderer.drawLinearGradient(
			{ allocation[ 2 ] - origin.x, origin.y, allocation[ 2 ], dimensions.y },
			{ allocation[ 2 ] - origin.x, ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ), allocation[ 2 ], ( origin.y + ( ( dimensions.y - origin.y ) / 2 ) ) },
			{ 0, 0, 0, 128 },
			{ 0, 0, 0, 0 }
		);
		// Bottom segment
		renderer.drawLinearGradient(
			{ origin.x, dimensions.y, allocation[ 2 ] - origin.x, allocation[ 3 ] },
			{ ( ( dimensions.x - origin.x ) / 2 ), dimensions.y, ( ( dimensions.x - origin.x ) / 2 ), allocation[ 3 ] },
			{ 0, 0, 0, 128 },
			{ 0, 0, 0, 0 }
		);
		// Bottom left corner
		renderer.drawScissored( { 0, dimensions.y, origin.x, allocation[ 3 ] }, [ & ]() {
			renderer.drawRadialGradient( { origin.x, dimensions.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
		} );
		// Bottom right corner
		renderer.drawScissored( { dimensions.x, dimensions.y, allocation[ 2 ], allocation[ 3 ] }, [ & ]() {
			renderer.drawRadialGradient( { dimensions.x, dimensions.y }, 0.05f, 4.95f, { 0, 0, 0, 128 }, { 0, 0, 0, 0 } );
		} );

		// Background color
		renderer.drawRect(
			glm::uvec4{ origin.x, origin.y, dimensions.x, dimensions.y },
			localStyle.get< glm::uvec4 >( "background-color" )
		);


		double fontSize = localStyle.get< double >( "font-size" );
		std::string font = localStyle.get< std::string >( "font" );
		int padding = localStyle.get< int >( "padding" );
		int verticalPosition = 5 + padding;

		for( const auto& item : items ) {
			std::visit( overloaded {
				[ & ]( const Item& item ) {
					/*
					glm::vec4 size = renderer.getTextSizeParams( font, item.label, fontSize );

					renderer.drawText(
						font,
						item.label,
						glm::uvec2 {
							( allocation[ 2 ] / 2 ) - ( size[ 2 ] / 2 ),
							verticalPosition + ( size[ 3 ] / 2.0f )
						},
						localStyle.get< glm::uvec4 >( "font-color" ),
						fontSize
					);

					verticalPosition += size[ 3 ] + padding;
					*/
				},
				[]( const Divider& divider ) { /* Nothing */ }
			}, item );
		}
	}
}