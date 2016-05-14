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
	local desired_tick = bluebear.engine.current_tick + 1
	local descriptor = self.object:register_callback( desired_tick, func_name, { ... } )

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

function TimedPromise:then_call( func_name, ... )
	local descriptor = self.object:register_callback( self.next_tick, func_name, { ... } )

	self.last.tick = self.next_tick
	self.last.callback = descriptor

	-- any future "then_call" statements will be called tick per tick
	self.next_tick = self.next_tick + 1

	return self
end

bluebear.register_class( TimedPromise )
