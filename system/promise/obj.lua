--[[
	define promises: there may be several kinds of promises (e.g. timebased promises in pure Lua,
	or engine-based promises that require the C++ engine to signal for the callback)
--]]

bluebear.engine.require_modpack( "yaci" )

_classes.promise = {
	base = newclass()
}

function _classes.promise.base:init( obj_ref, start_tick )
	self.object = obj_ref
	self.next_tick = start_tick
end

function _classes.promise.base:then_call( func_name, ... )
	self.object:register_callback( self.next_tick, func_name, { ... } )

	-- any future "then_call" statements will be called tick per tick
	self.next_tick = self.next_tick + 1

	return self
end
