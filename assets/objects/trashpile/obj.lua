local trashpile = {

	catalog = {
		name = "bluebear trashpile",
		description = "static tile of a trashpile",
		price = 0
	},

	actions = {

	},
	
	-- This function will run when the object is first placed on the lot
	init = function( self, lot )
		self.stink = 0
	end,
	
	-- This function will run when the object is scheduled to update its status.
	main = function( self, lot ) 
		-- Do not re-execute for at least 43200 game ticks (12 simulated game hours)
		return 43200
	end
}


bluebear.register_object( "mipani.trashpile", trashpile )
