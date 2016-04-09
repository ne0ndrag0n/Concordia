-- Provides the root environment for Lua scripts within the BlueBear engine
-- DO NOT MODIFY THIS FILE, or else everything will get goofy

-- Import base library and subsequent libraries
JSON = require( "system/JSON" )
dofile( "system/yaci.lua" )
dofile( "system/bblib.lua" )

_classes = {};

bluebear = {

	register_class = function( identifier, class_table )

		local id = identifier:split( '.' )

		if #id == 0 then
			print( "Could not load class  \""..identifier.."\": Invalid identifier!" )
			return false
		end

		-- Slap the class into the _classes table, a central registry of all types of classes available to the game
		local currentObject = _classes
		local max = #id - 1

		for i = 1, max, 1 do
			if currentObject[ id[ i ] ] == nil then
				-- Object doesn't exist: create it
				currentObject[ id[ i ] ] = {}
			end

			currentObject = currentObject[ id[ i ] ]
		end

		currentObject[ id[ #id ] ] = class_table

		print( "Registered class "..identifier )
	end,

	get_class = function( identifier )
		local id = identifier:split( '.' )
		local currentObject = _classes
		local max = #id

		for i = 1, max, 1 do
			if currentObject[ id[ i ] ] == nil then
				-- Object doesn't exist: return nil
				return nil
			end

			currentObject = currentObject[ id[ i ] ]
		end

		return currentObject
	end,

	extend = function( identifier, class_table )
		local Class = bluebear.get_class( identifier )
		local SubClass = nil

		if Class ~= nil then
			SubClass = _bblib.extend( Class:subclass(), class_table )
		end

		return SubClass
	end,

	new_instance = function( identifier )
		local Class = bluebear.get_class( identifier )
		local instance = nil

		if Class ~= nil then
			-- new instance
			instance = Class();

			-- deep copy all tables
			for key, value in pairs( instance ) do
				if key ~= "super" and type( value ) == 'table' then
					instance[ key ] = _bblib.deep_copy( value )
				end
			end
			-- call the "setup" function as constructor
			-- which makes up for the crappiness of metatable inheritance
			instance:setup()
		end

		return instance
	end
};

dofile( "system/basetype.lua" )
-- this is where we verify functions are written properly
dofile( "system/debugtests.lua" )
