local path = ...
local fps = bluebear.util.get_fps()

local Panel = {
  animation_in_progress = false,
  ANIMATIONS = {
    OPEN = {
      fps = fps,
      duration = fps / 4,
      keyframes = {
        [ 0.0 ] = {
          frames = {
            top = -450
          }
        },
        [ fps / 4 ] = {
          interpolate = true,
          frames = {
            top = 0
          }
        }
      }
    },
    CLOSE = {
      fps = fps,
      duration = fps / 4,
      keyframes = {
        [ 0.0 ] = {
          frames = {
            top = 0
          }
        },
        [ fps / 4 ] = {
          interpolate = true,
          frames = {
            top = -450
          }
        }
      }
    }
  },
  LOG_MESSAGE_TEMPLATE = [[
  <Layout class="-bb-log-message">
    <Text class="-bb-date %s">%s</Text>
    <Spacer class="buffer-log"></Spacer>
    <Text class="-bb-message">%s</Text>
  </Layout>
  ]],
  LEVEL_CLASSES = {
    [ 'd' ] = 'debug',
    [ 'i' ] = 'info',
    [ 'w' ] = 'warning',
    [ 'e' ] = 'error'
  },
  pane = nil,
  scrollback_bin = nil,
  system_event = nil
}

function Panel:load()
  bluebear.gui.load_stylesheet( { path..'/panel.style' } )
  self.pane = bluebear.gui.load_xml( path..'/panel.xml' )[ 1 ]
  bluebear.gui.root_element:add_child( self.pane )

  bluebear.event.register_key( '`', bluebear.util.bind( self.toggle, self ) )
  self.pane:set_style_property( 'top', -450 )

  self.scrollback_bin = self.pane:get_elements_by_class( { '-bb-scrollback-bin' } )[ 1 ]
  self.system_event = bluebear.event.register_system_event( 'message-logged', bluebear.util.bind( self.on_log, self ) )
end

function Panel:on_log( message )
  -- Prevent loopback
  bluebear.event.unregister_system_event( 'message-logged', self.system_event )

  local segment = bluebear.util.split( message, '[' )
  self.scrollback_bin:add_child(
    bluebear.gui.load_xml(
      string.format(
        self.LOG_MESSAGE_TEMPLATE,
        self.LEVEL_CLASSES[ string.sub( segment[ 1 ], 2, 2 ) ],
        segment[ 1 ],
        '['..segment[ 2 ]
      )
    , false )[ 1 ]
  )

  -- Restore
  self.system_event = bluebear.event.register_system_event( 'message-logged', bluebear.util.bind( self.on_log, self ) )
end

function Panel:toggle()
  if self.animation_in_progress == false then
    self.animation_in_progress = true
    local closed = self.pane:get_style_property( 'top' ) == -450

    if closed == true then
      -- Closed to Open
      self.pane:attach_animation( self.ANIMATIONS.OPEN, function()
        self.animation_in_progress = false
        self.pane:set_style_property( 'top', 0 )
      end )
    else
      -- Open to Closed
      self.pane:attach_animation( self.ANIMATIONS.CLOSE, function()
        self.animation_in_progress = false
        self.pane:set_style_property( 'top', -450 )
      end )
    end
  end
end

Panel:load()
