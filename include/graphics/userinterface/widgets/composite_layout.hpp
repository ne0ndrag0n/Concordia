#ifndef COMPOSITE_LAYOUT
#define COMPOSITE_LAYOUT

#include "graphics/userinterface/element.hpp"
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::UserInterface::Widgets {

    class CompositeLayout : public Element {
    protected:
        CompositeLayout( const std::string& id, const std::vector< std::string >& classes );

    public:
		void positionAndSizeChildren() override;
		bool drawableDirty() override;
		void generateDrawable() override;
		void calculate() override;

		static std::shared_ptr< CompositeLayout > create( const std::string& id, const std::vector< std::string >& classes );
    };

}

#endif