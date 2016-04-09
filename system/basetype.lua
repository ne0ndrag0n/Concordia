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
	print( "Warning: Object did not implement the load() method!" )
end

function _classes.object:save()
	return "{}";
end

function _classes.object:setup()
	self.cid = _bblib.get_cid()
end
