bluebear.engine.require_modpack( "trashpile" )

local Doll = bluebear.get_class( "system.doll.base" )
local Flowers = bluebear.extend( "system.entity.base", "game.flowers.base", {

	water_level = nil,

	catalog = {
		name = "Testico Violets",
		description = "Plant these beautiful flowers into your home!",
		price = 19.99
	},

	-- Here's a thorough explanation of the concept of BlueBear doll-object interactions, using the plant
	-- object as a reference implementation.
	interactions = {
		{
			-- This is the label that appears on an interaction
			-- It can either be a plaintext label, or a function running with object context
			-- e.g. "Water Ashley's Plant"
			label = "Water",
			-- This condition returns true if the interaction is available based on conditions between
			-- the given doll and the given object.
			condition = function( doll, object )
				return object.water_level ~= 0
			end,
			-- If true, this interaction will not appear on an object's context menu in the game UI. This
			-- is useful to define interactions which are exclusively autonomous
			hidden = false,
			-- This function is called once when the interacting doll switches its state to Doll.STATES.PREPARING
			-- This is normally where you bring the doll into the room, in front of the object, or otherwise set
			-- the stage for the doll to be switched into the interacting state
			prepare = function( doll, object )
				-- Simply walk to this object and call change_state to begin interacting with the object
				-- walk_to returns a promise that makes a request to the C++ Engine to find a path. Engine
				-- will use a thread pool to find the path, and the callback will be fired on the tick
				-- that is after the current tick when the thread pool task completes.
				doll:walk_to( object ):then_call( 'change_state', Doll.STATES.INTERACTING )
			end,
			-- This function is called once when the interacting doll switches its state to Doll.STATES.INTERACTING
			-- This is normally where you conduct activities required to fulfill the interaction. If it's a simple
			-- interaction, you can do everything on one promise from either the doll or the object. If it's a complex
			-- interaction or a looping interaction, you may want to start a new vthread (function that loops every n ticks)
			-- and track the doll on the object instance to ensure the doll can be released from the INTERACTING state
			interact = function( doll, object )
				-- Play the doll animation that features it watering an object in front of it. This unrolls a series
				-- of doll:change_graphic() callbacks across the next few ticks which will change the doll's appearance.
				-- After those animations are played, call the replenish_water method which will increase the water_level
				-- back to 100 and then change the object frame to healthy plants if necessary. After *that* completes,
				-- call change_state on the doll to CONCLUDING to give the doll its reward
				doll:animate( Doll.ANIMATIONS.WATERING_CAN )
					:then_call_on( object, 'replenish_water' )
					:then_call( 'change_state', Doll.STATES.CONCLUDING )
			end,
			-- This function is called once when the interacting doll switches its state to Doll.STATES.CONCLUDING
			-- Here, you grant the doll its motive reward and release it from the interaction by switching it back
			-- to Doll.STATES.IDLE
			conclude = function( doll, object )
				-- Reward the user with a few small points of belonging. Notice how the advertised belonging (see below)
				-- can differ from the actual belonging rewarded.
				doll:update_motive( 'game.motive.emotion.belonging', 5 ):then_call( 'change_state', Doll.STATES.IDLE )
			end,
			-- This function aborts an interaction. An interaction is aborted only when the user clicks an active
			-- interaction in the game UI, signaling to the doll that it needs to cancel immediately and return to
			-- idle. This function will be called once when the interacting doll receives this event and switches
			-- its state to Doll.STATES.ABORTING. Clean up all future callbacks, end vthreads, and conclude without
			-- the motive reward.
			abort = function( doll, object )
				-- Watering a plant isn't something that should be aborted since it's such a short interaction.
				-- Ignore the abort by doing nothing, and just allow the interaction to complete normally.
			end,
			-- Dolls, when idle and searching for things to do, will seek to satiate their motives first. The most
			-- important need based on weight and level will be pressed to be made higher. The doll in an idle loop
			-- asks objects around it what can satisfy its most important need best. This table exposes an interaction
			-- to the doll's questioning, telling the doll what it can do for its needs.

			-- This object advertises to dolls that Watering the plant will increase their Belonging emotional motive
			-- by 50 points. This, however, is untrue and false advertising. It ropes in depressed dolls with a high
			-- promise, giving them only a belonging increase of 5.
			advertisements = {
				motives = {
					['game.motive.emotion.belonging'] = 20
				}
			}
		}
	}
} )

function Flowers:main()
	print( "Hello from Lua! I am object instance ("..self._cid..") and my water level is now "..self.water_level )

	if self.water_level > 0 then
		self.water_level = self.water_level - 10
	end

	self:sleep( bluebear.util.time.minutes_to_ticks( 5 ) ):then_call( 'main' )
end

function Flowers:load( saved )
	bluebear.get_class( 'system.entity.base' ).load( self, saved )

	self.water_level = saved.water_level
end

function Flowers:replenish_water()
	self.water_level = 100
	self:change_graphic( 0, 0, Flowers.GRAPHICS.ALIVE )
end

-- Test function - the flowers turn into a trashpile if not watered
function Flowers:create_trashpile()
	local instance = bluebear.lot.create_new_instance( 'game.household.trashpile.base' )

	print( "Instance "..instance._cid.." created!" )
end
bluebear.register_class( Flowers )
