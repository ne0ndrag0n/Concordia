#include "gameplay/household/userinterface.hpp"
#include "graphics/userinterface/widgets/floating_pane.hpp"
#include "graphics/userinterface/widgets/grid_layout.hpp"
#include "graphics/userinterface/widgets/text.hpp"
#include "state/householdgameplaystate.hpp"
#include <string>

/**
 * The syspanel stuff is not intended to be modified or changed at runtime - hence its placement as a compile-time constant.
 */
static const std::string UI_XML = R"(
	<FloatingPane class="-bb-system-panel">
		<GridLayout class="-bb-syspanel-layout">
			<GridLayout class="-bb-syspanel-timedate-layout">
				<Text class="-bb-syspanel-time">12:00 AM</Text>
				<Text class="-bb-syspanel-date">Thu Jan 1</Text>
			</GridLayout>
			<GridLayout class="-bb-syspanel-rotate-zoom">
				<Button>LR</Button>
				<Button>RR</Button>
				<Button>+</Button>
				<Button>-</Button>
			</GridLayout>
		</GridLayout>
	</FloatingPane>
)";

static const std::string UI_CSS = R"(
	.-bb-system-panel {
		left: subtract( getIntSetting( "viewport_x" ), 310 );
		top: 10;
		width: 300;
		height: 150;

		.-bb-syspanel-layout {
			grid-rows: createLayout( 3, 1 );
			padding: 3;

			.-bb-syspanel-timedate-layout {
				grid-rows: createLayout( 1, 1 );

				Text {
					font: "lcd-italic";
					font-size: 20.0;
					background-color: rgbaString( "000000FF" );
					color: rgbaString( "00FFFFFF" );
					text-orientation-vertical: Orientation::MIDDLE;
				}
			}
		}

		.-bb-syspanel-rotate-zoom {
			grid-columns: createLayout( 1, 1, 1, 1 );
		}
	}
)";

namespace BlueBear::Gameplay::Household {

	UserInterface::UserInterface( State::HouseholdGameplayState& parentState ) : parentState( parentState ) {}

	void UserInterface::setup() {
		parentState.getGuiComponent().loadStylesheetSnippet( UI_CSS );

		auto resultArray = parentState.getGuiComponent().addElementsFromXML( UI_XML, false );
		if( resultArray.empty() ) {
			Log::getInstance().error( "UserInterface::setup", "Failed to load XML for user interface!" );
			return;
		}

		parentState.getGuiComponent().addElement( resultArray[ 0 ] );
	}

	Json::Value UserInterface::save() {
		return Json::Value::null;
	}

	void UserInterface::load( const Json::Value& data ) {}
}