local TEST_MODEL = 'bgb'
local TEST_ANIMATION = 'Armature|ArmatureAction.002'
local origin = bluebear.util.types.Vec2.new( -9.5, 9.5 )
local dimensions = bluebear.util.types.Vec2.new( 10.0, 10.0 )

local Profiler = {
	instances = nil
}

function Profiler:init()
	self.instances = {}
	bluebear.profiler = self
	print( 'system.component.model_profiler', 'Profiler attached at bluebear.profiler' )
end

function Profiler:position()
	local model_manager = self:get_model_manager()
	for y = dimensions.y, 0, -1 do
		for x = 0, dimensions.x do
			local new_object = model_manager:place_object( TEST_MODEL, {} )
			new_object:get_transform():set_position( bluebear.util.types.Vec3.new( origin.x + ( x * 2 ), origin.y - ( y * 2 ), 0.0 ) )
			table.insert( self.instances, new_object )
		end
	end
end

function Profiler:animate_all()
	for i, model in ipairs( self.instances ) do
		model:set_current_animation( TEST_ANIMATION )
	end
end

function Profiler:get_model_manager()
	local matches = self:get_entity():find_components( 'system.component.model_manager' )

	if #matches > 0 then
		return matches[ 1 ]
	else
		return nil
	end
end

bluebear.entity.register_component( 'system.component.model_profiler', Profiler )
bluebear.entity.register_entity( 'system.entity.profiler', { 'system.component.model_profiler' } )