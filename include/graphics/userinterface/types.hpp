#ifndef ALL_GUI_TYPES
#define ALL_GUI_TYPES

#include "graphics/userinterface/widgets/layout.hpp"
#include "graphics/userinterface/widgets/text.hpp"
#include "graphics/userinterface/widgets/window.hpp"
#include "graphics/userinterface/widgets/button.hpp"
#include "graphics/userinterface/widgets/input.hpp"
#include "graphics/userinterface/widgets/tablayout.hpp"
#include "graphics/userinterface/widgets/image.hpp"
#include "graphics/userinterface/widgets/spacer.hpp"
#include "graphics/userinterface/widgets/pane.hpp"
#include "graphics/userinterface/widgets/scroll.hpp"
#include "graphics/userinterface/widgets/context_menu.hpp"
#include "graphics/userinterface/widgets/floating_pane.hpp"
#include "graphics/userinterface/widgets/grid_layout.hpp"
#include <memory>
#include <variant>

namespace BlueBear::Graphics::UserInterface {

  using UIType = std::variant<
    std::shared_ptr< Element >,

    std::shared_ptr< Widgets::Layout >,
    std::shared_ptr< Widgets::Text >,
    std::shared_ptr< Widgets::Window >,
    std::shared_ptr< Widgets::WindowDecoration >,
    std::shared_ptr< Widgets::Button >,
    std::shared_ptr< Widgets::Input >,
    std::shared_ptr< Widgets::TabLayout >,
    std::shared_ptr< Widgets::Image >,
    std::shared_ptr< Widgets::Spacer >,
    std::shared_ptr< Widgets::Pane >,
    std::shared_ptr< Widgets::Scroll >,
    std::shared_ptr< Widgets::ContextMenu >,
    std::shared_ptr< Widgets::FloatingPane >,
    std::shared_ptr< Widgets::GridLayout >
  >;

}

#endif
