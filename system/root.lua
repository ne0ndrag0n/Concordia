-- Provides the root environment for Lua scripts within the BlueBear engine
-- DO NOT MODIFY THIS FILE, or else everything will get goofy

-- Import base library and subsequent libraries
dofile( "system/bblib.lua" )
JSON = require( "system/JSON" )

_classes = {};

bluebear = {

	register_class = function( identifier, object_table )

		print( "Registering class "..identifier )

		local id = identifier:split( '.' )

		if #id == 0 then
			print( "Could not load class  \""..identifier.."\": Invalid identifier!" )
			return false
		end

		-- "Class-ify" the object_table into something we can instantiate
		object_table.new = function()
			local self = setmetatable( {}, { __index = object_table } )
			return self
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

		currentObject[ id[ #id ] ] = object_table
	end
};

dofile( "system/basetype.lua" )
