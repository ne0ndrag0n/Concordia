local flowers = {

	water_level = nil,

	catalog = {
		name = "Testico Violets",
		description = "Plant these beautiful flowers into your home!",
		price = 19.99
	},

	actions = {
		{
			label = "Water",
			condition = function( self, player, lot )
				return self.water_level ~= 0
			end,
			action = function( self, player, lot )
				self.water_level = 100
				print( "Watered. Water level now "..self.water_level )
			end
		},
		{
			label = "Replant",
			condition = function( self, player, lot )
				return self.water_level == 0
			end,
			action = function( self, player, lot )
				self.water_level = 100
				print( "Replanted" )
			end
		}
	},

	init = function( self, lot )
		self.water_level = 100
	end,

	main = function( self, lot )

		print( "Hello from Lua! I am object instance ("..self._cid..") and my water level is now "..self.water_level )

		if self.water_level > 0 then
			self.water_level = self.water_level - 10
		end

		-- Do not re-execute for at least 43200 game ticks (12 simulated game hours)
		return 43200
	end
}


bluebear.register_object( "mipani.flowers", flowers )
