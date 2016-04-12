-- Base Types

-- todo

-- Register default traits
--[[
bluebear.register_motive( "hunger", {
	name = "Hunger",
	decay_rate = 10,
	decay_period = 6000,
	events = {
		{
			trigger = "<= 0",
			callback = function( self, lot )
				self.die()
			end
		}
	}
} )
--]]

_classes.object = newclass();

function _classes.object:load( saved )
	local data = saved or {}

	self._cid = data._cid
	self._sys = data._sys
end

function _classes.object:save()
	local out = {
		_sys = self._sys,
		_cid = self._cid
	}

	return out
end

function _classes.object:setup()
	-- abstract - This is the "real" constructor of objects
end
