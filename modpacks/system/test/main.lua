print( ..., 'This is a modpack' )

-- Load demo components
bluebear.event.register_key( 'v', function()
  bluebear.gui.load_xml( 'system/ui/example.xml' )

  bluebear.gui.get_elements( { { tag = 'Window' } } )
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
