bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "entity" )

local modpack_path = ...
local GUIProvider = bluebear.extend( 'system.entity.base', 'system.provider.gui' )

function GUIProvider:open_debug_ui()
  local path = modpack_path.."/debug.xml"
  local console_path = modpack_path.."/console.xml"

  print( "system.provider.gui", "Providing the debug UI from "..path )

  self.gui = bluebear.gui.create_gui_context( path )
  self.gui:add_from_path( console_path )

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
  local element = self.gui:find_by_id( 'bb_console' )

  local interval = 0
  local initial = element:get_property( 'top' )
  local final
  local step

  if initial == -360 then
    final = 0
    step = 20
  else
    final = -360
    step = -20
  end

  for i=initial,final,step do
    interval = interval + 1
    self:sleep( interval ):then_call( function() element:set_property( 'top', i ) end )
  end
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
