bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "entity" )

local modpack_path = ...
local GUIProvider = bluebear.extend( 'system.entity.base', 'system.provider.gui' )

function GUIProvider:open_debug_ui()
  local path = modpack_path.."/debug.xml"
  local console_path = modpack_path.."/console.xml"

  print( "system.provider.gui", "Providing the debug UI from "..path )

  bluebear.gui.add_from_path( path )
  bluebear.gui.add_from_path( console_path )

  bluebear.gui.find_by_id( "rot_l" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_rot_l", self ) )
  bluebear.gui.find_by_id( "rot_r" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_rot_r", self ) )
  bluebear.gui.find_by_id( "zoom_in" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_zoom_in", self ) )
  bluebear.gui.find_by_id( "zoom_out" ):on( "click", bluebear.util.bind( "system.provider.gui:on_click_zoom_out", self ) )

  bluebear.gui.find_by_id( "ta1" ):on( "click", bluebear.util.bind( "system.provider.gui:test_action_1", self ) )
  bluebear.gui.find_by_id( "ta2" ):on( "click", bluebear.util.bind( "system.provider.gui:test_action_2", self ) )

  bluebear.gui.find_by_id( "toggle_table" ):on( "click", bluebear.util.bind( "system.provider.gui:toggle_visibility", self ) )

  local chat = bluebear.gui.find_by_id( 'bb_chatscroller' )
  local chatwidth = chat:get_property( 'width' )

  bluebear.gui.find_by_id( "bb_ritzy" ):set_property(
    'fixed_x',
    ( chatwidth / 2 ) - 150
  )

  print( chat:get_style( "FontSize" ) )
  print( chatwidth )
  self.cl_line_chars = bluebear.util.round( ( chatwidth - 100 ) / ( tonumber( chat:get_style( "FontSize" ) / 2 ) + 0.5 ) )
  print( self.cl_line_chars )

  -- XXX: Remove after demo
  bluebear.gui.find_by_id( "animate1" ):on( "click", bluebear.gui.__internal__playanim1 )
end

function GUIProvider:test_action_1( event )
  local element = bluebear.gui.find_by_id( 'bb_console' )

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
  local content = 'fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart fart FART FART'
  local line_template = [[
    <Alignment scale_x="0" scale_y="0">
      <Label>%s</Label>
    </Alignment>
  ]]
  local textarea = bluebear.gui.find_by_id( 'bb_textarea' )

  -- writing this drunk and tired
  local finished = false
  local lbound = 1

  while finished == false do
    local ubound = lbound + self.cl_line_chars

    if ubound > content:len() then
      -- last one
      textarea:add( string.format( line_template, content:sub( lbound, content:len() ) ) )
      finished = true
    else
      textarea:add( string.format( line_template, content:sub( lbound, ubound ) ) )
      lbound = lbound + self.cl_line_chars
    end
  end
end

function GUIProvider:toggle_visibility( event )
  local tablewindow = bluebear.gui.find_by_id( 'tablewindow' )

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
