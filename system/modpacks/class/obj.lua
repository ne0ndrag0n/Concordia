--[[
  Set up the class system used by the game
--]]

bluebear.engine.require_modpack( "middleclass" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "json" )

-- Container for registered classes in Luasphere
bluebear.classes = {}

--[[
	Register a new class in the bluebear system

	@param		{Class}			Class			A class.
--]]
bluebear.register_class = function( Class )
	local id = bluebear.util.split( Class.name, '.' )

	if #id == 0 then
		print( "bluebear.register_class", "Could not load class  \""..Class.name.."\": Invalid identifier!" )
		return false
	end

	-- Slap the class into the bluebear.classes table, a central registry of all types of classes available to the game
	local currentObject = bluebear.classes
	local max = #id - 1

	for i = 1, max, 1 do
		if currentObject[ id[ i ] ] == nil then
			-- Object doesn't exist: create it
			currentObject[ id[ i ] ] = {}
		end

		currentObject = currentObject[ id[ i ] ]
	end

	-- Mark every class with this hidden property to signify that this table is a class
	-- This distinguishes between classes and namespaces in seek operations
	Class.__middleclass = true

	currentObject[ id[ #id ] ] = Class

	print( "bluebear.register_class", "Registered class "..Class.name )
end

bluebear.get_class = function( identifier )
	local id = bluebear.util.split( identifier, '.' )
	local currentObject = bluebear.classes
	local max = #id

	for i = 1, max, 1 do
		if currentObject[ id[ i ] ] == nil then
			-- Object doesn't exist: return nil
			return nil
		end

		currentObject = currentObject[ id[ i ] ]
	end

	return currentObject
end

--[[
	Get classes of type "identifier". This will return not only classes that ARE the base class,
	but classes that derive from the specified base class. Potentially expensive operation:
	make sure you cache its results (new classes should not be registered in the course of
	a game.)

	@param	{String}		identifier		The base class
	@param	{Boolean}		include_base	Include the base class in the list of returned classes
--]]
local function recursive_helper( node, Needle, found_classes )
	-- Leaf node
	if node.__middleclass then
		if node:isSubclassOf( Needle ) then table.insert( found_classes, node ) end
	else
		-- Ordinary node
		for key, value in pairs( node ) do
			recursive_helper( value, Needle, found_classes )
		end
	end
end
bluebear.get_classes_of_type = function( identifier, include_base )
	-- Get the class to compare to
	local Needle = bluebear.get_class( identifier )

	-- If it doesn't exist, we have nothing else to do here.
	if Needle == nil then return nil end

	-- Use the recursive_helper function to navigate bluebear.classes,
	-- placing the results in at every step of the direction.
	local found_classes = {}
	if include_base == true then table.insert( found_classes, Needle ) end
	recursive_helper( bluebear.classes, Needle, found_classes )

	return found_classes
end

--[[
	Extend a class registered using bluebear.register_class

	@param	{String}		identifier		The class you wish to extend
	@param	{String}		new_name			The name of the new class
	@param	{Table}			class_table		The class table
--]]
bluebear.extend = function( identifier, new_name, class_table )
  local Class = bluebear.get_class( identifier )
  local SubClass = nil

  if Class ~= nil then
    SubClass = bluebear.util.extend( Class:subclass( new_name ), class_table )
  end

  return SubClass
end

--[[
  Check if a given object is an instance of "identifier"

  @param		{String}		identifier		The identifier for the class to compare to
  @param		{Object}		instance
--]]
bluebear.instance_of = function( identifier, instance )
  local Class = bluebear.get_class( identifier )

  if Class == nil then
    return false
  end

  return instance:isInstanceOf( Class )
end

-- Plugins to util

--[[
  Binds "instance-type" functions (functions which accept instance as first argument). This is part of the class module, as its functionality
  depends on class for "safe" serialization.
--]]
bluebear.util.bind = function( f, ... )
  local func
  local args = { ... }

	local __derived_class
	local __derived_func

	if type( f ) == "string" then
		local split = bluebear.util.split( f, ':' )
		__derived_class = split[ 1 ]
		__derived_func = split[ 2 ]

		-- Complete func
		func = bluebear.get_class( __derived_class )[ __derived_func ]
	else
		func = f

		-- These will just be nil if they don't exist
		__derived_class = bluebear.util.get_upvalue_by_name( func, "__derived_class" )
		__derived_func = bluebear.util.get_upvalue_by_name( func, "__derived_func" )
	end

  return function( ... )
    local newargs = { ... }
    local allargs = {}

		-- Hack for upvalue serialization. When __derived_class and __derived_func are not nil, these are used to
		-- create a "sfunction" entry when saving the game state. "sfunction" entries are better because they
		-- allow you to upgrade modpacks without risking changes in behaviour when the game next loads.
		local __derived_class = __derived_class
		local __derived_func = __derived_func

    for k,v in pairs( args ) do
      table.insert( allargs, v )
    end

    for k,v in pairs( newargs ) do
      table.insert( allargs, v )
    end

		-- 2/19/17 we now return result of function, possible oversight from before
    return func( table.unpack( allargs ) )
  end
end
