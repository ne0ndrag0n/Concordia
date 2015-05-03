-- This is the first BlueBear object to be tested!

-- Objects must take the form of a table passed to bluebear.register_object with at least the following properties:
-- init: This function is run when the object is first placed on the lot. Sets crucial attributes about the object
-- main: This function is called when the object is due to be refreshed. It returns the amount to sleep for at least, in world seconds.
-- The following are optional properties
-- actions: If not nil, a table of actions taking the form { label, condition, action } where label is the pie menu label, condition is when the option appears, and action is the action taken
local flowers = {

	water_level = nil,

	actions = {
		{
			label = "Water",
			condition = function( self, player, lot )
				return self.water_level ~= 0
			end,
			action = function( self, player, lot ) 
				print( "Watering" )
				self.water_level = 100
				print( "Watered" )
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
	
	-- This function will run when the object is first placed on the lot
	init = function( self, lot )
		self.water_level = 100
	end,
	
	-- This function will run when the object is scheduled to update its status.
	main = function( self, lot ) 
		if self.water_level > 0 
			self.water_level = self.water_level - 10
		end
		
		-- Do not re-execute for at least 43200 game ticks (12 simulated game hours)
		return 43200
	end
}



bluebear.register_object( flowers )
