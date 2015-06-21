-- Provides the root environment for Lua scripts within the BlueBear engine
-- DO NOT MODIFY THIS FILE, or else everything will get goofy

dofile( "system/bblib.lua" )

-- Central registry of bbobjects (not the object instances used per-lot)
_bbobjects = {};

-- Central registry of bluebear templates
_obj_templates = {};

bluebear = {

	Constants = {
		POPTYPES = {
			"_bbobjects",
			"_bbplayers"
		}
	},

	register_object = function( identifier, object_table ) 
		
		local id = identifier:split( '.' )
		
		-- Assertions
		-- Identifier must have an author and a mod descriptor separated by a period
		if #id ~= 2 then
			print( "Could not load BBObject \""..identifier.."\": Invalid identifier!" )
			return false
		end
		
		-- Object must not already be loaded (or identifier must not be used)
		if type( _bbobjects[ identifier ] ) ~= "nil" then
			print( "Could not load BBObject \""..identifier.."\": Namespace collision!" )
			return false
		end
		
		-- The "init" function must be present
		if type( object_table.init ) ~= "function" then
			print( "Could not load BBObject \""..identifier.."\": Invalid \"init\" function!" )
			return false
		end
		
		-- The "main" function must be present
		if type( object_table.main ) ~= "function" then
			print( "Could not load BBObject \"" ..identifier.."\": Invalid \"main\" function!" )
			return false
		end
		
		-- The "catalog" table must be present, and must contain a name, description, and price
		if type( object_table.catalog ) ~= "table" then
			print( "Could not load BBObject \""..identifier.."\": Invalid \"catalog\" table!" )
			return false
		else
			-- Catalog assertions
			-- Catalog should have a name property
			if type( object_table.catalog.name ) ~= "string" then
				print( "Could not load BBObject \""..identifier.."\": Catalog has invalid \"name\" property!" )
				return false
			end
			
			-- Catalog should have description property
			if type( object_table.catalog.description ) ~= "string" then
				print( "Could not load BBObject \""..identifier.."\": Catalog has invalid \"description\" property!" )
				return false
			end
			
			-- Catalog should have a price property
			if type( object_table.catalog.price ) ~= "number" then
				print( "Could not load BBObject \""..identifier.."\": Catalog has invalid \"price\" property!" )
				return false
			end
		end
		
		-- Table fits the expected pattern!
		-- "Class-ify" the object_table into something we can instantiate
		object_table.__index = object_table
		object_table.new = function() 
			local self = setmetatable( {}, object_table )
			return self
		end
		
		-- Slap the class into the _bbobjects table, a central registry of all objects available to the game
		_bbobjects[ identifier ] = object_table
		print( "Loaded object "..identifier )
	end,
	
	-- No verification is being done here - that'll be caught by bluebear.register_object
	register_object_template = function( identifier, template_table ) 
		if template_table ~= nil then
			_obj_templates[ identifier ] = template_table
			print( "Registered object template "..identifier )
			return _obj_templates[ identifier ]
		else
			print( "Invalid template table for identifier "..identifier )
			return false
		end
	end,
	
	register_object_from_template = function( template_key, identifier, object_table )
		local template_obj = _obj_templates[ template_key ]
		
		-- If there is no template object, we cannot continue
		if template_obj == nil then
			print( "Template "..template_key.." was not found!" )
			return false
		end
		
		-- Step 1: Simple extend
		local extended = _bblib.extend_object( template_obj, object_table )
				
		-- Step 2: Concatenate the actions array, if it exists in both template_actions and object_table.actions
		-- If we don't define a new set of actions, use the actions available in the template
		if object_table.actions ~= nil then
			if template_obj.actions ~= nil then
				extended.actions = _bblib.concatenate_arrays( template_obj.actions, object_table.actions )
			end
		else
			extended.actions = template_obj.actions
		end
		
		-- Step 3: Register the object we just extended
		bluebear.register_object( identifier, extended )
	end,
	
	call_super_object = function( id, func_id, is_template ) 
		local returned_func
		
		if is_template == true then
			returned_func = _obj_templates[ id ][ func_id ]
		else
			returned_func = _bbobjects[ id ][ func_id ]
		end
		
		if type( returned_func ) == "function" then
			return returned_func
		end
	end,
	
	call_super_player = function( id, func_id, is_template )
		local returned_func
	
		if is_template == true then
			return _plr_templates[ id ][ func_id ]
		else 
			return _bbplayers[ id ][ func_id ]
		end
		
		if type( returned_func ) == "function" then
			return returned_func
		end
	end
	
};

