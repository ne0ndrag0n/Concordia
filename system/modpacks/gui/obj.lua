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

  bluebear.gui.find_by_id( "bb_clear" ):on( "click", bluebear.util.bind( "system.provider.gui:clear_chat", self ) )
  bluebear.gui.find_by_id( "bb_exec" ):on( "click", bluebear.util.bind( "system.provider.gui:handle_command", self ) )

  -- TODO: Start caching MarkupEngine DOM queries here
  self.console_input = bluebear.gui.find_by_id( 'bb_entry' )
  self.chat_scroller = bluebear.gui.find_by_id( 'bb_chatscroller' )
  self.console_input:on( "key_down", bluebear.util.bind( "system.provider.gui:check_enter_press", self ) )

  -- XXX: Remove after demo
  bluebear.gui.find_by_id( "animate1" ):on( "click", bluebear.gui.__internal__playanim1 )

  self:determine_max_chat_chars()
  self.OUTPUT_COLORS = {
    [ '(d)' ] = 'bb_green',
    [ '(i)' ] = 'bb_blue',
    [ '(w)' ] = 'bb_yellow',
    [ '(e)' ] = 'bb_red'
  }

  bluebear.event.register_key( '~', bluebear.util.bind( "system.provider.gui:toggle_visibility", self ) )
  bluebear.event.listen( 'MESSAGE_LOGGED', bluebear.util.bind( 'system.provider.gui:on_message_logged', self ) )
end

function GUIProvider:test_action_1( event )

end

function GUIProvider:test_action_2( event )

end

function GUIProvider:on_message_logged( message )
  self:echo( message )
end

function GUIProvider:clear_chat()
  local alignments = bluebear.gui.find_by_class( 'bb_chatline' )
  local textarea = bluebear.gui.find_by_id( 'bb_textarea' )

  for i, alignment in ipairs( alignments ) do
    textarea:remove( alignment )
  end
end

function GUIProvider:check_enter_press( event )
  if event.keyboard.key == 'ret' then
    self:handle_command()
  end
end

function GUIProvider:handle_command()
  local text = self.console_input:get_content()

  self:sleep( 1 ):then_call( load( text ) )

  self.console_input:set_content( '' )
end

function GUIProvider:determine_max_chat_chars()
  local chat = bluebear.gui.find_by_id( 'bb_chatscroller' )
  local chatwidth = chat:get_property( 'width' )

  bluebear.gui.find_by_id( "bb_ritzy" ):set_property(
    'fixed_x',
    ( chatwidth / 2 ) - 150
  )

  self.cl_line_chars = bluebear.util.round( ( chatwidth - 100 ) / ( tonumber( chat:get_style( "FontSize" ) / 2 ) + 0.5 ) )
end

function GUIProvider:toggle_visibility()
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

function GUIProvider:echo( content )
  local line_template = [[
    <Alignment class="bb_chatline" scale_x="0" scale_y="0">
      <Box>
        %s
        <Label>%s</Label>
      </Box>
    </Alignment>
  ]]
  local prefix_template = [[
    <Label class="%s">%s</Label>
  ]]
  local textarea = bluebear.gui.find_by_id( 'bb_textarea' )

  -- writing this drunk and tired
  local finished = false
  local lbound = 1

  while finished == false do
    local ubound = lbound + self.cl_line_chars

    if ubound > content:len() then
      -- last one
      textarea:add( string.format( line_template, '', content:sub( lbound, content:len() ) ) )
      finished = true
    else
      local xml_string

      if lbound == 1 then
        xml_string = string.format(
          line_template,
          string.format( prefix_template, self.OUTPUT_COLORS[ content:sub( 1, 3 ) ], content:sub( lbound, 25 ) ),
          content:sub( 26, ubound )
        )
      else
        xml_string = string.format( line_template, '', content:sub( lbound, ubound ) )
      end

      textarea:add( xml_string )
      lbound = lbound + self.cl_line_chars + 1
    end
  end

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
