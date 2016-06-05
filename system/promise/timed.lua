local Promise = ...
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
