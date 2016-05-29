local TrashPile = bluebear.extend( "system.entity.base", "game.household.trashpile.base", {

	catalog = {
		name = "bluebear trashpile",
		description = "static tile of a trashpile",
		price = 0
	}
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

function TrashPile:test_callback()
	print( "Instance "..self._cid.." had callback triggered successfully at tick "..tostring( bluebear.engine.current_tick ) )
end

function TrashPile:test_callback_individual()
	print( "Instance "..self._cid.." was listening to bb1 and got triggered from that object successfully, tick is "..tostring( bluebear.engine.current_tick ) )
end

bluebear.register_class( TrashPile )
