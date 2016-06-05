--[[
	define promises: there may be several kinds of promises (e.g. timebased promises in pure Lua,
	or engine-based promises that require the C++ engine to signal for the callback)
--]]


bluebear.engine.require_modpack( "middleclass" )
bluebear.engine.require_modpack( "class" )

local Promise = class( 'system.promise.base' )

function Promise:initialize( obj_ref )
	self.object = obj_ref

	-- self.last is modified after every then_call, and holds information pertaining
	-- to the last registered callback
	self.last = {
		tick = nil,
		callback = nil
	}
end

function Promise:then_call( func_name, ... )
	self:then_call_on( self.object, func_name, ... )
end

function Promise:then_call_on( entity, func_name, ... )
	local desired_tick = bluebear.engine.current_tick + 1
	local descriptor = entity:register_callback( desired_tick, func_name, { ... } )

	self.last.tick = desired_tick
	self.last.callback = descriptor

	return self
end

function Promise:and_get_reference()
	-- Do this here to save logic time in tight loop
	self.last.tick = tostring( self.last.tick )

	return self.last
end

bluebear.register_class( Promise )

-- Now register all the other types (Base Promise had to get done first)
local modpack_path = ...
local files = bluebear.util.get_directory_list( modpack_path )

for index, file in ipairs( files ) do
	if file.name ~= "obj.lua" then assert( loadfile( modpack_path.."/"..file.name ) )( Promise ) end
end
