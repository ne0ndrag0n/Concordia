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
