local Class = bluebear.extend( "system.object.base", {

	catalog = {
		name = "bluebear trashpile",
		description = "static tile of a trashpile",
		price = 0
	},

	actions = {

	},

	-- This function will run when the object is scheduled to update its status.
	main = function( self )
		print( "Hello from Lua! I am object instance ("..self._cid..")" )

		self:sleep( 86400 ):then_call( 'main' )
	end,

	load = function( self, saved )
		bluebear.get_class( 'system.object.base' ).load( self, saved )

		self.stink = saved.stink
	end
} )


bluebear.register_class( "game.household.trashpile.base", Class )
