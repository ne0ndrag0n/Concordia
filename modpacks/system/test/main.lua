print( ..., 'This is a modpack' )

local test = { a = 1, b = "two", c = { three = 3 } }
function test:fun() end
function test:init( arg )
  print( self.c )
  print( test.c )
  print( 'system.component.test', arg.test )
end

bluebear.entity.register_component( 'system.component.test', test )
bluebear.entity.register_entity( 'system.entity.test', { 'system.component.test' } )

bluebear.entity.create_new_entity( 'system.entity.test', {
  [ 'system.component.test' ] = { test = 'boop!' }
} )
