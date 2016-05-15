bluebear.engine.require_modpack( "trashpile" )

local Flowers = bluebear.extend( "system.entity.base", "game.flowers.base", {

	water_level = nil,

	catalog = {
		name = "Testico Violets",
		description = "Plant these beautiful flowers into your home!",
		price = 19.99
	},

	interactions = {
		{
			label = "Water",
			condition = function( doll, object )
				return object.water_level ~= 0
			end,
			action = function( doll, object )
				object.water_level = 100
				print( "Watered. Water level now "..self.water_level )
			end,
			advertisements = {
				['game.motive.emotion.belonging'] = 20
			}
		},
		{
			label = function( doll, object )
				-- Interaction labels can take the form of functions to generate strings
				-- e.g. "Water Ashley's Fish", "Pay Bills ($250)"
				return "Replant"
			end,
			condition = function( doll, object )
				return object.water_level == 0
			end,
			action = function( doll, object )
				object.water_level = 100
				print( "Replanted" )
			end
		},
		{
			label = "Admire",
			-- use this flag to denote an interaction that is hidden from the player
			-- on the object click menu
			hidden = true,
			action = function( doll, object )
				-- stuff goes here
			end,
			advertisements = {
				['game.motive.emotion.environment'] = 50
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

-- Test function - the flowers turn into a trashpile if not watered
function Flowers:create_trashpile()
	local instance = bluebear.lot.create_new_instance( 'game.household.trashpile.base' )

	print( "Instance "..instance._cid.." created!" )
end
bluebear.register_class( Flowers )
