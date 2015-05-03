-- This is the first BlueBear object to be tested!

local flowers = {

	dead = false,

	water_level = 100,

	-- These properties are used by the lot engine (to add to its ambient light level or aesthetic level)
	props = {
		AESTHETIC_SCORE = 10		
	},
	
	-- This property determines what will appear on the pie menu
	pie_menu = bluebear.utility.extend(
		bluebear.object.semiglobals.PLANT,
		{ 'View', 'view_condition', 'view_action' },
		{ 'Replant', 'replant_condition', 'replant_action' }
	),
	
	-- This property handles actions when pie menu items are selected
	actions = {
		view_action = function( self, player, lot )
			player.animate( bluebear.common.animations.OBSERVE, 5000 )
			player.motives.aesthetic = player.motives.aesthetic + self.props.AESTHETIC_SCORE
		end,
		water_action = function( self, player, lot )
			player.animate( bluebear.common.animations.WATER_PLANT, 3000 )
			self.water_level = 100
		end,
		replant_action = function( self, player, lot ) 
			player.animate( bluebear.common.animations.REPLANT, 10000 )
			self.water_level = 1000
			self.props.AESTHETIC_SCORE = 10
			bluebear.set_object_image_state( "mipani_flowers", self.assets.image.FLOWERS_ALIVE )
			self.dead = false
		end
	},
	
	-- This property determines WHEN items will appear on the pie menu
	conditions = {
		water_condition = function( self, player, lot )
			return self.water_level >= 1
		end,
		
		-- no view_condition - always true - always display pie menu item
		
		replant_condition = function( self, player, lot ) 
			return self.dead
		end
	},
	
	-- This function will run when the object is first placed on the lot
	init = function( self, lot )
		self.water_level = 100
	end,
	
	-- This function will run when the object is scheduled to update its status.
	main = function( self, lot ) {
		if self.water_level = 0 && self.dead == false
			self.props.AESTHETIC_SCORE = -50
			bluebear.set_object_image_state( "mipani_flowers", self.assets.image.FLOWERS_DEAD )
			self.dead = true
		elseif self.dead == false
			self.water_level = self.water_level - 10
			-- Die quicker without enough ambient light
			if lot.get_ambient_light() <= 30
				self.water_level = self.water_level - 30
			end
		end
		
		-- Do not re-execute for at least 43200 game ticks (12 simulated game hours)
		return 43200
	}
}



bluebear.register_object( "mipani_flowers", flowers )
