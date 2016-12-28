local TrashPile = bluebear.extend( "system.entity.base", "game.household.trashpile.base", {

	catalog = {
		name = "bluebear trashpile",
		description = "static tile of a trashpile",
		price = 0
	}
} )

-- This function will run when the object is scheduled to update its status.
function TrashPile:main()
	print( TrashPile.name, "Hello from Lua! I am object instance ("..self._cid..")" )

	self:sleep( bluebear.util.time.minutes_to_ticks( 1 ) ):then_call( bluebear.util.bind( 'game.household.trashpile.base:main', self ) )
end

bluebear.register_class( TrashPile )
