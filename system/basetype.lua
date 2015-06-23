-- Base Types

-- Register default traits

local base_player = {

	motives = {
		-- min and max are implicit values that default to 0 and 100 respectively. you can extend these
		{
			id = "hunger",
			name = "Hunger",
			decay_rate = 10,
			decay_period = 6000,
			events = {
				{
					trigger = "<= 0",
					callback = function( self, lot ) 
						self.die()
					end
				}
			}
		},
		{
			id = "hygiene",
			name = "Hygiene",
			decay_rate = 10,
			decay_period = 6000,
			events = {
				{
					trigger = "<= 15",
					recur = false,
					callback = function( self, lot )
						lot.modal_message( bluebear.Dialog.DIALOG_OK, "What is that...smell?" )
					end
				}
			}
		},
		{
			id = "bladder",
			name = "Bladder",
			decay_rate = 5,
			decay_period = 6000,
			increase = true,
			events = {
				{
					trigger = "= 100",
					callback = function( self, lot ) 
						lot.spawn_object( "mipani.puddle", bluebear.Location.BELOW_PLAYER )
						self.animate( self.Animations.EMBARASSED_1 )
						self.set_motive( "hygiene", 15 )
					end
				}
			}
		},
		{
			id = "energy",
			name = "Energy",
			decay_rate = 5,
			decay_period = 6000,
			events = {
				{
					trigger = "<= 5",
					callback = "sleep"
				}
			}
		}
	}

}

_obj_templates[ "base_player" ] = base_player
