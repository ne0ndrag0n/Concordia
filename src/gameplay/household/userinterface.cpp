#include "gameplay/household/userinterface.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/widgets/button.hpp"
#include "graphics/userinterface/widgets/floating_pane.hpp"
#include "graphics/userinterface/widgets/grid_layout.hpp"
#include "graphics/userinterface/widgets/text.hpp"
#include "graphics/camera.hpp"
#include "state/householdgameplaystate.hpp"
#include "log.hpp"
#include <string>
#include <functional>

/**
 * The syspanel stuff is not intended to be modified or changed at runtime - hence its placement as a compile-time constant.
 */
static const std::string UI_XML = R"(
	<FloatingPane class="-bb-system-panel">
		<GridLayout class="-bb-syspanel-layout">
			<GridLayout class="-bb-syspanel-timedate-layout">
				<Text class="-bb-syspanel-time">12:00 AM</Text>
				<GridLayout class="-bb-syspanel-date-budget">
					<Text class="-bb-syspanel-date">Thu Jan 1</Text>
					<Text class="-bb-syspanel-budget">$20,000</Text>
				</GridLayout>
			</GridLayout>
			<GridLayout class="-bb-syspanel-wall-settings">
				<Button>No Walls</Button>
				<Button>Cutaway</Button>
				<Button>Walls Up</Button>
				<Button>Roof</Button>
			</GridLayout>
			<GridLayout class="-bb-syspanel-rotate-zoom">
				<Button class="symbol-font -bb-rotate-right">&#xf01e;</Button>
				<Button class="symbol-font -bb-rotate-left">&#xf0e2;</Button>
				<Button class="symbol-font -bb-zoom-in">&#xf00e;</Button>
				<Button class="symbol-font -bb-zoom-out">&#xf010;</Button>
			</GridLayout>
			<GridLayout class="-bb-syspanel-play-pause">
				<Button class="symbol-font -bb-syspanel-pause">&#xf04c;</Button>
				<Button class="symbol-font -bb-syspanel-play">&#xf04b;</Button>
				<Button class="symbol-font -bb-syspanel-ff">&#xf050;</Button>
			</GridLayout>
		</GridLayout>
	</FloatingPane>
)";

static const std::string UI_CSS = R"(
	.-bb-system-panel {
		left: subtract( getIntSetting( "viewport_x" ), 310 );
		top: 10;
		width: 300;
		height: 200;

		.-bb-syspanel-layout {
			grid-rows: createLayout( 3, 1, 1, 1 );
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

				.-bb-syspanel-date-budget {
					grid-columns: createLayout( 1, 1 );

					Text.-bb-syspanel-budget {
						color: rgbaString( "00AA00FF" );
					}
				}
			}

			.-bb-syspanel-wall-settings {
				grid-columns: createLayout( 1, 1, 1, 1 );
			}

			.-bb-syspanel-rotate-zoom {
				grid-columns: createLayout( 1, 1, 1, 1 );
			}

			.-bb-syspanel-play-pause {
				grid-columns: createLayout( 1, 2, 1 );

				Button.-bb-syspanel-pause {
					background-color: rgbaString( "770000FF" );
					fade-in-color: rgbaString( "AB0000FF" );
					fade-out-color: rgbaString( "450000FF" );
				}
			}
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

		controlPanel = std::static_pointer_cast< Graphics::UserInterface::Widgets::FloatingPane >( resultArray[ 0 ] );

		std::shared_ptr< Graphics::UserInterface::Element > rootLayout = controlPanel->getChildren()[ 0 ];
		std::shared_ptr< Graphics::UserInterface::Element > rotateZoomLayout = rootLayout->getChildren()[ 2 ];

		Graphics::Camera& camera = parentState.getWorldRenderer().getCamera();

		rotateRight = std::static_pointer_cast< Graphics::UserInterface::Widgets::Button >( rotateZoomLayout->getChildren()[ 0 ] );
		rotateRight->getEventBundle().registerInputEvent( "mouse-up", std::bind( &Graphics::Camera::rotateRight, &camera ) );

		rotateLeft = std::static_pointer_cast< Graphics::UserInterface::Widgets::Button >( rotateZoomLayout->getChildren()[ 1 ] );
		rotateLeft->getEventBundle().registerInputEvent( "mouse-up", std::bind( &Graphics::Camera::rotateLeft, &camera ) );

		zoomIn = std::static_pointer_cast< Graphics::UserInterface::Widgets::Button >( rotateZoomLayout->getChildren()[ 2 ] );
		zoomIn->getEventBundle().registerInputEvent( "mouse-up", std::bind( &Graphics::Camera::zoomIn, &camera ) );

		zoomOut = std::static_pointer_cast< Graphics::UserInterface::Widgets::Button >( rotateZoomLayout->getChildren()[ 3 ] );
		zoomOut->getEventBundle().registerInputEvent( "mouse-up", std::bind( &Graphics::Camera::zoomOut, &camera ) );
	}

	Json::Value UserInterface::save() {
		return Json::Value::null;
	}

	void UserInterface::load( const Json::Value& data ) {}
}