#ifndef CONCORDIA_GUI_CONTEXT_MENU
#define CONCORDIA_GUI_CONTEXT_MENU

#include "graphics/userinterface/element.hpp"
#include <variant>
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::UserInterface::Widgets {

	class ContextMenu : public Element {
	public:
		struct Item {
			std::string id;
			std::string label;
		};

		enum class Divider;

		using Entry = std::variant< Item, Divider >;

	private:
		std::vector< Entry > items;
		double longestTextSpan = 0.0f;
		double textHeight = 0.0f;

		std::string getLongestLabel() const;

	protected:
		ContextMenu( const std::string& id, const std::vector< std::string >& classes, const std::vector< Entry >& items );

	public:
		void calculate() override;

		static std::vector< Entry > parseEntries( const std::string& text );
		static std::shared_ptr< ContextMenu > create( const std::string& id, const std::vector< std::string >& classes, const std::vector< Entry >& items );
	};

}

#endif