-- Provides the root environment for Lua scripts within the BlueBear engine
-- DO NOT MODIFY THIS FILE, or else everything will get goofy

-- Import base library and subsequent libraries
JSON = require( "system/json/obj" )
dofile( "system/yaci/obj.lua" )

_classes = {};

bluebear = {

	-- Table containing methods to interface with the BlueBear lot (a primarily C++ construct)
	lot = nil,

	-- Table containing properties of the current engine (for now, just the current tick) usable by internal methods
	engine = {
		current_tick = 0
	},

	-- Contains utility functions (formerly _bblib)
	util = {
		lastcid = 0,

		extend = function( destination, ... )
			local arg = { ... }

			-- Overwrite with shallow references to each source
			for index, source in ipairs( arg ) do
				for key, value in pairs( source ) do
					destination[ key ] = value
				end
			end

			return destination
		end,

		-- Courtesy lua-users wiki: Copy Table - http://lua-users.org/wiki/CopyTable
		deep_copy = function( orig )
		    local orig_type = type(orig)
		    local copy
		    if orig_type == 'table' then
		        copy = {}
		        for orig_key, orig_value in next, orig, nil do
		            copy[bluebear.util.deep_copy(orig_key)] = bluebear.util.deep_copy(orig_value)
		        end
		        setmetatable(copy, bluebear.util.deep_copy(getmetatable(orig)))
		    else -- number, string, boolean, etc
		        copy = orig
		    end
		    return copy
		end,

		concatenate_arrays = function( first, last )
			local new_table = {}

			for k,v in pairs( first ) do
				table.insert( new_table, v )
			end

			for k,v in pairs( last ) do
				table.insert( new_table, v )
			end

			return new_table
		end,

		get_cid = function()
			bluebear.util.lastcid = bluebear.util.lastcid + 1
			return "bb"..bluebear.util.lastcid
		end,

		split = function( str, delimiter )
			local delimiter, fields = delimiter or ":", {}
			local pattern = string.format("([^%s]+)", delimiter)
			str:gsub(pattern, function(c) fields[#fields+1] = c end)
			return fields
		end
	},

	register_class = function( identifier, class_table )

		local id = bluebear.util.split( identifier, '.' )

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
		local id = bluebear.util.split( identifier, '.' )
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
			SubClass = bluebear.util.extend( Class:subclass(), class_table )
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
				if key ~= 'super' and type( value ) == 'table' then
					instance[ key ] = bluebear.util.deep_copy( value )
				end
			end
			-- call the "setup" function as constructor
			-- which makes up for the crappiness of metatable inheritance
			instance:setup()
		end

		return instance
	end,

	--[[
		Create a new instance from a serialised version of the object.

		@param		{String}		identifier		The object class which will be selected from _classes
		@param		{String}		savedInstance	The saved instance that will need to be converted back to a Lua table
	--]]
	new_instance_from_file = function( identifier, savedInstance )
		-- May use JSON or a Lua string table serialisation (I'm thinking of getting rid of JSON)
		local savedInstanceTable = JSON:decode( savedInstance )
		local instance = bluebear.new_instance( identifier )

		if instance == nil then error( identifier.." is not a registered class. (did you install the mod under the assets/ directory?)" ) end

		-- deserialise
		instance:load( savedInstanceTable )

		-- and that's it!
		return instance
	end,

	--[[
		Check if a given object is an instance of "identifier"

		@param		{String}		identifier		The identifier for the class to compare to
		@param		{Object}		instance		An instance of at least 'object'
	--]]
	instance_of = function( identifier, instance )
		local Class = bluebear.get_class( identifier )

		if Class == nil then
			return false
		end

		return Class:made( instance )
	end
};

-- Define basic types
dofile( "system/base_game/obj.lua" )
