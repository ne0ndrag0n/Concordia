-- Provides the root environment for Lua scripts within the BlueBear engine
-- DO NOT MODIFY THIS FILE, or else everything will get goofy

dofile( "system/bblib.lua" )

-- Central registry of bbobjects (not the object instances used per-lot)
_bbobjects = {};

--[[
	Object "instances" that are used by the currently loaded lot go here
]]--
_lotinsts = {};

bluebear = {

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
		object_table.instantiate = function() 
			local instance = {}
			setmetatable( instance, object_table )
			return instance
		end
		
		-- Slap the class into the _bbobjects table, a central registry of all objects available to the game
		_bbobjects[ identifier ] = object_table
		print( "Loaded object "..identifier )
	end
	
};

