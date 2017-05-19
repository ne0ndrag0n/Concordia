bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "entity" )

local modpack_path = ...
local GUIProvider = bluebear.extend( 'system.entity.base', 'system.provider.gui' )

function GUIProvider:open_debug_ui()
  local path = modpack_path.."/debug.xml"
  print( "system.provider.gui", "Providing the debug UI from "..path )

  self.gui = bluebear.gui.create_gui_context( path )

  self.gui:find_by_id( "rot_l" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_rot_l", self ) )
  self.gui:find_by_id( "rot_r" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_rot_r", self ) )
  self.gui:find_by_id( "zoom_in" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_zoom_in", self ) )
  self.gui:find_by_id( "zoom_out" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_zoom_out", self ) )

  self.gui:find_by_id( "ta1" ):on( "click", bluebear.util.bind( "system.provider.gui:test_action_1", self ) )
  self.gui:find_by_id( "ta2" ):on( "click", bluebear.util.bind( "system.provider.gui:test_action_2", self ) )

  self.gui:find_by_id( "toggle_table" ):on( "click", bluebear.util.bind( "system.provider.gui:toggle_visibility", self ) )

  -- XXX: Remove after demo
  self.gui:find_by_id( "animate1" ):on( "click", bluebear.gui.__internal__playanim1 )
end

function GUIProvider:test_action_1( event )
  local combo = self.gui:find_by_id( 'combo' )
  local newitem = [[
    <item>potato</item>
  ]]

  local item2 = combo:find_pseudo( 'item', 2 )

  combo:add( newitem, 0 )
end

function GUIProvider:test_action_2( event )

end

function GUIProvider:toggle_visibility( event )
  local tablewindow = self.gui:find_by_id( 'tablewindow' )

  tablewindow:set_property( 'visible', not tablewindow:get_property( 'visible' ) )
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

bluebear.register_class( GUIProvider )
