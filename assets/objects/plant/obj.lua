-- This is the first BlueBear object to be tested!

-- Objects must take the form of a table passed to bluebear.register_object with at least the following properties:
-- init: This function is run when the object is first placed on the lot. Sets crucial attributes about the object
-- main: This function is called when the object is due to be refreshed. It returns the amount to sleep for at least, in world seconds.
-- catalog: This table contains the catalog properties (must contain at least name, description, price)
-- The following are optional properties
-- actions: If not nil, a table of actions taking the form { label, condition, action } where label is the pie menu label, condition is when the option appears, and action is the action taken
-- marshal/unmarshal: save and load a table that sets the object state when the game is saved or loaded
local flowers = {

	-- Don't specify this manually: this represents what the BlueBear VM adds to every object
	_sys = {
		-- Object execution schedule
		_schedule = nil
	},

	water_level = nil,

	catalog = {
		name = "bluebear flowers",
		description = "these are bluebear flowers",
		price = 19.99
	},

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
		if self.water_level > 0 then
			self.water_level = self.water_level - 10
		end
		
		-- Do not re-execute for at least 43200 game ticks (12 simulated game hours)
		return 43200
	end,
	
	marshal = function( self )
		local state = {
			water_level = self.water_level
		}
		
		return state
	end,
	
	unmarshal = function( self, data ) 
		self.water_level = data.water_level
	end
}


-- Register your object by calling register_object on the global bluebear object, and passing in the table containing the required values
-- BlueBear enforces unique object names in a convention: author.object
bluebear.register_object( "mipani.flowers", flowers )
