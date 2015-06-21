local flowers = {

	catalog = {
		name = "Testico Violets",
		description = "Plant these beautiful flowers into your home!",
		price = 19.99
	},
	
	main = function( self, lot ) 
		local ticks_delay = bluebear.super_object( 'potted_plant', 'main', true )( self, lot )
		
		return ticks_delay
	end
}


bluebear.register_object_from_template( "potted_plant", "mipani.flowers", flowers )
