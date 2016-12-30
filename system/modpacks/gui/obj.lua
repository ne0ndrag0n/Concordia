bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "entity" )

local modpack_path = ...
local GUIProvider = bluebear.extend( 'system.entity.base', 'system.provider.gui' )

function GUIProvider:open_debug_ui()
  local path = modpack_path.."/debug.xml"
  print( "system.provider.gui", "Providing the debug UI from "..path )

  bluebear.gui.load_widgets( path )
  bluebear.gui.get_widget_by_id( "rot_l" ):on( "click", function() print( "Called!" ) end )
end

function GUIProvider:handle_error( error )
  print( "system.provider.gui", error )
end

bluebear.register_class( GUIProvider )
