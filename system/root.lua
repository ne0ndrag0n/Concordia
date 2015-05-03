-- Provides the root environment for Lua scripts within the BlueBear engine
-- DO NOT MODIFY THIS FILE

_bbobjects = {};

bluebear = {

	object = {
	
		semiglobals = {
			PLANT = {
				WATER = { 'Water', 'water_condition', 'water_action' }
			}
		},
		
		register_object = function() 
			-- todo
		end
	}

	utility = {
		extend = function( t1, t2 )
			--for k,v in ipairs( t2 ) do table.insert( t1, v ) end return t1
			--concatenate tables if possible
		end
	}
}
