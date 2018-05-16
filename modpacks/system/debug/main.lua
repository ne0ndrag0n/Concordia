local path = ...
local Panel = {
  animation_in_progress = false,
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
  -- Closed to Open
  self.pane:attach_animation( {
    fps = 60.0,
    duration = 30.0,
    suicide = false,
    sticky = true,
    keyframes = {
      [ 0.0 ] = {
        frames = {
          top = -450
        }
      },
      [ 30.0 ] = {
        interpolate = true,
        frames = {
          top = 0
        }
      }
    }
  } )
end

Panel:load()
