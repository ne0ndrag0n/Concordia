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

--------------------------------------------------------------------------------

local TimedPromise = bluebear.extend( 'system.promise.base', 'system.promise.timer' )

function TimedPromise:initialize( obj_ref, start_tick )
	Promise.initialize( self, obj_ref )

	self.next_tick = start_tick
end

function TimedPromise:then_call_on( entity, func_name, ... )
	local descriptor = entity:register_callback( self.next_tick, func_name, { ... } )

	self.last.tick = self.next_tick
	self.last.callback = descriptor

	-- any future "then_call" statements will be called tick per tick
	self.next_tick = self.next_tick + 1

	return self
end

bluebear.register_class( TimedPromise )

--------------------------------------------------------------------------------

-- TODO: PathfinderPromise calls methods on bluebear.engine to find a path and call back

local PathfinderPromise = bluebear.extend( 'system.promise.base', 'system.promise.pathfinder' )

function PathfinderPromise:initialize( origin, target )
	-- origin - self.origin
	-- target - self.object
	-- Pathfinder finds path from origin to target
	Promise.initialize( self, target )

	self.origin = origin

	-- then_call will put these onto the promise, when the engine gets done finding the path it will
	-- run through this table and call 'em all, each waiting one tick between them all.
	self.thens = {}

	-- TODO: the actual thing. right now this just does a simple defer. consider this for Picasso milestone.
	-- the below snippet doesn't go in this method, it goes somewhere else
	--[[
	bluebear.engine.find_path( self.origin, self.object, function( result )
 		-- do the shit where we unload "result" as a series of object navigations across the map
		-- now call all your thens
		for index, callback in ipairs( self.thens ) do

		end
	end )
	--]]
end

bluebear.register_class( PathfinderPromise )
