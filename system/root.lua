-- Provides the root environment for Lua scripts within the BlueBear engine
-- DO NOT MODIFY THIS FILE, or else everything will get goofy

function string:split(sep)
	local sep, fields = sep or ":", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

_bbobjects = {};

bluebear = {

	register_object = function( identifier, object_table ) 
		
		local id = identifier.split( '.' )
		
		-- Assertions
		-- Identifier must have an author and a mod descriptor separated by a period
		if table.getn( id ) ~= 2 then
			print( "Could not load BBObject \"" + identifier + "\": Invalid identifier!" )
			return false
		end
		
		-- Object must not already be loaded (or identifier must not be used)
		if type( _bbobjects[ identifier ] ) ~= "nil" then
			print( "Could not load BBObject \"" + identifier + "\": Namespace collision!" )
			return false
		end
		
		-- The "init" function must be present
		if type( object_table.init ) ~= "function" then
			print( "Could not load BBObject \"" + identifier + "\": Missing \"init\" function!" )
			return false
		end
		
		-- The "main" function must be present
		if type( object_table.main ) ~= "function" then
			print( "Could not load BBObject \"" + identifier + "\": Missing \"main\" function!" )
			return false
		end
		
		_bbobjects[ identifier ] = object_table
		print( "Loaded object " + identifier )
	end

};

