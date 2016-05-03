--[[
  Set up the class system used by the game
--]]

bluebear.engine.require_modpack( "middleclass" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "json" )

-- Container for registered classes in Luasphere
bluebear.classes = {}

bluebear.register_class = function( identifier, class_table )
	local id = bluebear.util.split( identifier, '.' )

	if #id == 0 then
		print( "Could not load class  \""..identifier.."\": Invalid identifier!" )
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

	currentObject[ id[ #id ] ] = class_table

	print( "Registered class "..identifier )
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

bluebear.extend = function( identifier, class_table )
  local Class = bluebear.get_class( identifier )
  local SubClass = nil

  if Class ~= nil then
    SubClass = bluebear.util.extend( Class:subclass( identifier ), class_table )
  end

  return SubClass
end

bluebear.new_instance = function( identifier )
  local Class = bluebear.get_class( identifier )
  local instance = nil

  if Class ~= nil then
    -- new instance
    instance = Class:new();

    -- call the "setup" function as "actual" constructor
    instance:setup()
  end

  return instance
end

--[[
  Create a new instance from a serialised version of the object.

  @param		{String}		identifier		The object class which will be selected from bluebear.classes
  @param		{String}		savedInstance	The saved instance that will need to be converted back to a Lua table
--]]
bluebear.new_instance_from_file = function( identifier, savedInstance )
  -- May use JSON or a Lua string table serialisation (I'm thinking of getting rid of JSON)
  local savedInstanceTable = JSON:decode( savedInstance )
  local instance = bluebear.new_instance( identifier )

  if instance == nil then error( identifier.." is not a registered class. (did you install the mod under the assets/ directory?)" ) end

  -- deserialise
  instance:load( savedInstanceTable )

  -- and that's it!
  return instance
end

--[[
  Check if a given object is an instance of "identifier"

  @param		{String}		identifier		The identifier for the class to compare to
  @param		{Object}		instance			An instance of at least 'system.object.base'
--]]
bluebear.instance_of = function( identifier, instance )
  local Class = bluebear.get_class( identifier )

  if Class == nil then
    return false
  end

  return instance:isInstanceOf( Class )
end
