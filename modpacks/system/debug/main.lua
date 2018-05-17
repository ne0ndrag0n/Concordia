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
  pane = nil
}

function Panel:load()
  bluebear.gui.load_stylesheet( { path..'/panel.style' } )
  self.pane = bluebear.gui.load_xml( path..'/panel.xml' )[ 1 ]
  bluebear.gui.root_element:add_child( self.pane )

  bluebear.event.register_key( '`', bluebear.util.bind( self.toggle, self ) )
  self.pane:set_style_property( 'top', -450 )
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
