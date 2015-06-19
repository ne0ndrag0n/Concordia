local flowers = {

	catalog = {
		name = "Testico Violets",
		description = "Plant these beautiful flowers into your home!",
		price = 19.99
	},
	
	main = function( self, lot ) 
		local rv = self.super.main( self, lot )
		
		print( "meep" )
		
		return rv
	end
}


bluebear.register_object_from_template( "potted_plant", "mipani.flowers", flowers )
