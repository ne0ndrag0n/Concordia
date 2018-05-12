print( ..., 'This is a modpack' )

local test = { a = 1, b = "two", c = { three = 3 } }
function test:fun() print( 'programming is fun '..tostring( self.a ) ) end
function test:init( arg )
  print( self.c )
  print( test.c )
  print( 'system.component.test', arg.test )
end

bluebear.entity.register_component( 'system.component.test', test )
bluebear.entity.register_entity( 'system.entity.test', { 'system.component.test' } )

local entity = bluebear.entity.create_new_entity( 'system.entity.test', {
  [ 'system.component.test' ] = { test = 'boop!' }
} )

local component = entity:get_component( "system.component.test" )
print( entity == component:get_entity() )

entity = nil
print( 'preparing to trash' )
collectgarbage()

print( 'now this should throw...' )
print( entity == component:get_entity() )
