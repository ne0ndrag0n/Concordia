bluebear.engine.require_modpack( "trashpile" )

local Class = bluebear.extend( "system.entity.base", "game.flowers.base", {

	water_level = nil,

	catalog = {
		name = "Testico Violets",
		description = "Plant these beautiful flowers into your home!",
		price = 19.99
	},

	actions = {
		{
			label = "Water",
			condition = function( self )
				return self.water_level ~= 0
			end,
			action = function( self )
				self.water_level = 100
				print( "Watered. Water level now "..self.water_level )
			end
		},
		{
			label = "Replant",
			condition = function( self )
				return self.water_level == 0
			end,
			action = function( self )
				self.water_level = 100
				print( "Replanted" )
			end
		}
	},

	main = function( self )

		print( "Hello from Lua! I am object instance ("..self._cid..") and my water level is now "..self.water_level )

		if self.water_level > 0 then
			self.water_level = self.water_level - 10
		end

		print( "Sleeping for 5 game minutes..." )
		self:sleep( bluebear.util.time.minutes_to_ticks( 5 ) ):then_call( 'main' )
	end,

	load = function( self, saved )
		bluebear.get_class( 'system.entity.base' ).load( self, saved )

		self.water_level = saved.water_level
	end
} )

bluebear.register_class( Class )
