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
		<!--
		<GridLayout class="-bb-syspanel-layout">
			<GridLayout class="-bb-syspanel-timedate-layout">
				<Text class="-bb-syspanel-time">12:00 AM</Text>
				<Text class="-bb-syspanel-date">Thu Jan 1 1987</Text>
			</GridLayout>
			<GridLayout class="-bb-syspanel-rotate-zoom">
				<Button>LR</Button>
				<Button>RR</Button>
				<Button>+</Button>
				<Button>-</Button>
			</GridLayout>
		</GridLayout>
		-->
	</FloatingPane>
)";

static const std::string UI_CSS = R"(
	.-bb-system-panel {
		left: subtract( getIntSetting( "viewport_x" ), 310 );
		top: 10;
		width: 300;
		height: 300;
	}
)";

namespace BlueBear::Gameplay::Household {

	UserInterface::UserInterface( State::HouseholdGameplayState& parentState ) : parentState( parentState ) {}

	void UserInterface::setup() {

	}

	Json::Value UserInterface::save() {
		return Json::Value::null;
	}

	void UserInterface::load( const Json::Value& data ) {}
}