print( ..., 'This is a modpack' )

-- Load demo components
bluebear.event.register_key( 'v', function()
  local items = bluebear.gui.load_xml( 'system/ui/example.xml' )

  for i, element in ipairs( items ) do
    bluebear.gui.root_element:add_child( element )
  end
end )

bluebear.event.register_key( 'b', function()
  local decoration = bluebear.gui.get_elements( { { tag = 'WindowDecoration' } } )[ 1 ]

  print( decoration:get_selector_string() )
  decoration:attach_animation( {
    fps = 60.0,
    duration = 300.0,
    keyframes = {
      [ 0.0 ] = {
        frames = {
          color = bluebear.util.types.Uvec4.new( 0, 0, 0, 255 )
        }
      },
      [ 150.0 ] = {
        interpolate = true,
        frames = {
          color = bluebear.util.types.Uvec4.new( 0, 0, 128, 255 )
        }
      },
      [ 300.0 ] = {
        interpolate = true,
        frames = {
          color = bluebear.util.types.Uvec4.new( 0, 0, 255, 255 )
        }
      }
    }
  } )
end )

function testcode()
  local test = { a = 1, b = "two", c = { three = 3 } }
  function test:fun() print( 'programming is fun '..tostring( self.a ) ) end
  function test:init( arg )
    print( self.c )
    print( test.c )
    print( 'system.component.test', arg.test )
  end

  bluebear.entity.register_component( 'system.component.test', test )
  bluebear.entity.register_entity( 'system.entity.test', { 'system.component.test', 'system.component.model_manager' } )

  local entity = bluebear.entity.create_new_entity( 'system.entity.test', {
    [ 'system.component.test' ] = { test = 'boop!' },
    [ 'system.component.model_manager' ] = { 'game.models.flower' },
  } )
end
