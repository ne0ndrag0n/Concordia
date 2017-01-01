bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "entity" )

local modpack_path = ...
local GUIProvider = bluebear.extend( 'system.entity.base', 'system.provider.gui' )

function GUIProvider:open_debug_ui()
  local path = modpack_path.."/debug.xml"
  print( "system.provider.gui", "Providing the debug UI from "..path )

  bluebear.gui.load_widgets( path )

  bluebear.gui.get_widget_by_id( "rot_l" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_rot_l", self ) )
  bluebear.gui.get_widget_by_id( "rot_r" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_rot_r", self ) )
  bluebear.gui.get_widget_by_id( "zoom_in" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_zoom_in", self ) )
  bluebear.gui.get_widget_by_id( "zoom_out" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_zoom_out", self ) )
end

function GUIProvider:on_click_zoom_in()
  bluebear.gui.zoom_in()
end

function GUIProvider:on_click_zoom_out()
  bluebear.gui.zoom_out()
end

function GUIProvider:on_click_rot_l()
  bluebear.gui.rotate_left()
end

function GUIProvider:on_click_rot_r()
  bluebear.gui.rotate_right()
end

function GUIProvider:handle_error( error )
  print( "system.provider.gui", error )
end

bluebear.register_class( GUIProvider )
