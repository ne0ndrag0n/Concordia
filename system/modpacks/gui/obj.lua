bluebear.engine.require_modpack( 'class' )
bluebear.engine.require_modpack( 'util' )
bluebear.engine.require_modpack( 'entity' )

local modpack_path = ...
local GUIProvider = bluebear.extend( 'system.entity.base', 'system.provider.gui' )

function GUIProvider:open_debug_ui()
  bluebear.gui.load_theme( modpack_path..'/chatarea.theme' )
  bluebear.gui.add_from_path( modpack_path..'/debug.xml' )
  bluebear.gui.add_from_path( modpack_path..'/console.xml' )

  self:cache_queries()
  self:set_callbacks()
  self:set_constants()
  self:hook_events()
end

function GUIProvider:cache_queries()
  self.console_input = bluebear.gui.find_by_id( 'bb_console_entry' )
  self.chat_scroller = bluebear.gui.find_by_id( 'bb_console_chatscroller' )
  self.bg_image = bluebear.gui.find_by_id( 'bb_console_ritzy' )
  self.textarea = bluebear.gui.find_by_id( 'bb_console_textarea' )
  self.console_window = bluebear.gui.find_by_id( 'bb_console_console' )
end

function GUIProvider:set_callbacks()
  bluebear.gui.find_by_id( 'bb_console_clear' ):on( 'click', bluebear.util.bind( 'system.provider.gui:clear_chat', self ) )
  bluebear.gui.find_by_id( 'bb_console_exec' ):on( 'click', bluebear.util.bind( 'system.provider.gui:handle_command', self ) )

  bluebear.gui.find_by_id( 'bb_debug_gc' ):on( 'click', bluebear.util.bind( 'system.provider.gui:run_gc', self ) )
  bluebear.gui.find_by_id( 'bb_debug_print_ram_usage' ):on( 'click', bluebear.util.bind( 'system.provider.gui:print_kbytes', self ) )
  bluebear.gui.find_by_id( 'bb_debug_toggle_console' ):on( 'click', bluebear.util.bind( 'system.provider.gui:toggle_visibility', self ) )
end

function GUIProvider:print_kbytes()
  print( 'system.provider.gui', collectgarbage( 'count' )..' KB' )
end

function GUIProvider:run_gc()
  print( 'system.provider.gui', 'Collecting garbage...' )
  self:print_kbytes()
  collectgarbage()
  self:print_kbytes()
  print( 'system.provider.gui', 'Done' )
end

function GUIProvider:set_constants()
  self.natural_height = self.chat_scroller:get_property( 'max_y' ) - self.chat_scroller:get_property( 'scroll_y' )
  self:determine_max_chat_chars()
  self.OUTPUT_COLORS = {
    [ '(d)' ] = 'bb_green',
    [ '(i)' ] = 'bb_blue',
    [ '(w)' ] = 'bb_yellow',
    [ '(e)' ] = 'bb_red'
  }
  self.LINE_TEMPLATE = io.open( modpack_path..'/line_fragment.xml' ):read( '*all' )
  self.PREFIX_TEMPLATE = io.open( modpack_path..'/prefix_fragment.xml' ):read( '*all' )

  self.command_history = {}
  self.command_history_index = 0
end

function GUIProvider:hook_events()
  bluebear.event.register_key( '~', bluebear.util.bind( 'system.provider.gui:toggle_visibility', self ) )
  bluebear.event.listen( 'MESSAGE_LOGGED', bluebear.util.bind( 'system.provider.gui:echo', self ) )

  self.console_input:on( 'key_down', bluebear.util.bind( 'system.provider.gui:check_enter_press', self ) )
end

function GUIProvider:clear_chat()
  local alignments = self.textarea:get_children()

  for i, alignment in ipairs( alignments ) do
    self.textarea:remove( alignment )
  end
end

function GUIProvider:check_enter_press( event )
  if event.keyboard.key == 'ret' then
    self:handle_command()
  elseif event.keyboard.key == 'up' then
    self:show_previous_command()
  elseif event.keyboard.key == 'down' then
    self:show_next_command()
  end
end

function GUIProvider:show_previous_command()
  local prev = self.command_history_index - 1

  if prev >= 1 then
    self.console_input:set_content( self.command_history[ prev ] )
    self.command_history_index = prev
  end
end

function GUIProvider:show_next_command()
  local next = self.command_history_index + 1

  if next <= #self.command_history then
    self.console_input:set_content( self.command_history[ next ] )
    self.command_history_index = next
  else
    self.console_input:set_content( '' )
    self.command_history_index = #self.command_history + 1
  end
end

function GUIProvider:handle_command()
  local text = self.console_input:get_content()
  table.insert( self.command_history, text )
  self.command_history_index = #self.command_history + 1

  self:sleep( 1 ):then_call( load( text ) )

  self.console_input:set_content( '' )
end

function GUIProvider:determine_max_chat_chars()
  local chatwidth = self.chat_scroller:get_property( 'width' )

  self.bg_image:set_property(
    'fixed_x',
    ( chatwidth / 2 ) - 150
  )

  self.cl_line_chars = bluebear.util.round( ( chatwidth - 100 ) / ( tonumber( self.chat_scroller:get_style( 'FontSize' ) / 2 ) + 0.5 ) )
end

function GUIProvider:toggle_visibility()
  local interval = 0
  local initial = self.console_window:get_property( 'top' )
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
    self:sleep( interval ):then_call( function() self.console_window:set_property( 'top', i ) end )
  end
end

function GUIProvider:echo( content )
  local scroll_down =
    ( self.chat_scroller:get_property( 'max_y' ) - self.chat_scroller:get_property( 'scroll_y' ) == self.natural_height )
      or
    ( self.chat_scroller:get_property( 'max_y' ) > self.natural_height and self.chat_scroller:get_property( 'scroll_y' ) == 0 )
  local max_lines = tonumber( bluebear.config.get_value( 'max_ingame_terminal_scrollback' ) ) or 100

  local lines = bluebear.util.split( content, '\n' )
  for i, line in ipairs( lines ) do
    local splits = self:get_lines( line )

    for j, split in ipairs( splits ) do
      local textarea_children = self.textarea:get_children()

      if #textarea_children > max_lines then
        -- Start removing older scrollback
        self.textarea:remove( textarea_children[ 1 ] )
      end

      self.textarea:add( split )
    end
  end

  if scroll_down == true then
    self.chat_scroller:set_property( 'scroll_y', self.chat_scroller:get_property( 'max_y' ) )
  end
end

--[[
  Look, it's a mess, but don't touch it.
--]]
function GUIProvider:get_lines( content )
  local finished = false
  local lbound = 1
  local xml_string
  local results = {}

  while finished == false do
    local ubound = lbound + self.cl_line_chars

    if ubound > content:len() then
      -- last one
      if lbound == 1 and content:sub( 1, 1 ) == '(' then
        xml_string = string.format(
          self.LINE_TEMPLATE,
          string.format( self.PREFIX_TEMPLATE, self.OUTPUT_COLORS[ content:sub( 1, 3 ) ], content:sub( lbound, 25 ) ),
          content:sub( 26, ubound )
        )
      else
        xml_string = string.format( self.LINE_TEMPLATE, '', content:sub( lbound, content:len() ) )
      end

      table.insert( results, xml_string )
      finished = true
    else
      if lbound == 1 and content:sub( 1, 1 ) == '(' then
        xml_string = string.format(
          self.LINE_TEMPLATE,
          string.format( self.PREFIX_TEMPLATE, self.OUTPUT_COLORS[ content:sub( 1, 3 ) ], content:sub( lbound, 25 ) ),
          content:sub( 26, ubound )
        )
      else
        xml_string = string.format( self.LINE_TEMPLATE, '', content:sub( lbound, ubound ) )
      end

      table.insert( results, xml_string )
      lbound = lbound + self.cl_line_chars + 1
    end
  end

  return results
end

bluebear.register_class( GUIProvider )
