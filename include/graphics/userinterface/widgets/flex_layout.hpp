#ifndef FLEX_LAYOUT
#define FLEX_LAYOUT

#include "graphics/userinterface/element.hpp"
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::UserInterface::Widgets {

	// TODO: Three steps to integrate into element engine
	class FlexLayout : public Element {
	protected:
		FlexLayout( const std::string& id, const std::vector< std::string >& classes );

		bool isHorizontal() const;
		bool isForward() const;
		glm::ivec2 getDirection() const;
		glm::ivec2 getElementsSize() const;
		glm::ivec2 getLiteralSize( const std::shared_ptr< Element >& child ) const;
		glm::ivec2 getOrigin() const;

	public:
		void positionAndSizeChildren() override;
		bool drawableDirty() override;
		void generateDrawable() override;
		void calculate() override;

		static std::shared_ptr< FlexLayout > create( const std::string& id, const std::vector< std::string >& classes );
	};

}

#endif