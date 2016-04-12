local Class = bluebear.extend( 'object', {

	catalog = {
		name = "bluebear trashpile",
		description = "static tile of a trashpile",
		price = 0
	},

	actions = {

	},

	-- This function will run when the object is first placed on the lot
	setup = function( self )
		self.super:setup()

		self.stink = 0
	end,

	-- This function will run when the object is scheduled to update its status.
	main = function( self )
		print( "Hello from Lua! I am object instance ("..self._cid..")" )

		local all_objects = bluebear.lot.get_objects_by_type( "game.flowers.base" )

		print( "I will deduct water_level in BlueBear object instance "..all_objects[1]._cid.." down to 50." )
		all_objects[1].water_level = 50

		return 86400
	end
} )


bluebear.register_class( "game.household.trashpile.base", Class )
