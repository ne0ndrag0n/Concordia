local TrashPile = bluebear.extend( "system.entity.base", "game.household.trashpile.base", {

	catalog = {
		name = "bluebear trashpile",
		description = "static tile of a trashpile",
		price = 0
	},

	actions = {

	},
} )

-- This function will run when the object is scheduled to update its status.
function TrashPile:main()
	print( "Hello from Lua! I am object instance ("..self._cid..")" )

	self:sleep( bluebear.util.time.minutes_to_ticks( 1 ) ):then_call( 'main' )
end

function TrashPile:load( saved )
	bluebear.get_class( 'system.entity.base' ).load( self, saved )

	self.stink = saved.stink
end

function TrashPile:on_create()
	bluebear.get_class( 'system.entity.base' ).on_create( self )

	print( "Hello from Lua! I am a SECOND trashpile - ("..self._cid..")" )
end

bluebear.register_class( TrashPile )
