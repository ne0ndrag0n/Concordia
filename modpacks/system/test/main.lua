print( ..., 'This is a modpack' )

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

local model_manager = entity:get_component( 'system.component.model_manager' )
print( model_manager:get_potential_models()[ 1 ] == 'game.models.flower' )
