function string:split(sep)
	local sep, fields = sep or ":", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

_bblib = {
	lastcid = 0,

	extend_object = function( parent, subclass )
		local new_table = {}
		
		-- Shallow copy parent to new_table
		for k,v in pairs( parent ) do
			new_table[ k ] = v
		end
	
		-- Overwrite with shallow references to subclass
		for k,v in pairs( subclass ) do
			new_table[ k ] = v
		end
		
		return new_table
	end,
	
	deep_copy_table = function( obj, seen )
	  if type( obj ) ~= 'table' then return obj end
	  if seen and seen[ obj ] then return seen[ obj ] end
	  local s = seen or {}
	  local res = setmetatable( {}, getmetatable( obj ) )
	  s[ obj ] = res
	  for k, v in pairs( obj ) do res[ _bblib.deep_copy_table( k, s ) ] = _bblib.deep_copy_table( v, s ) end
	  return res
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

	serialise = function( table, content ) 
		-- TODO
	end,
	
	--[[
		This method is very testy and could use some unit tests & refinement so it's not as testy
	]]--
	deserialise = function( bytes, byref ) 
		local tbl = {}
		byref = byref or { cursor = 1 }
		
		while true do
			-- get character at cursor position, then advance cursor to key
			local signal = bytes:sub( byref.cursor, byref.cursor )
			byref.cursor = byref.cursor + 1
			
			-- if signal is \xFF, then break
			if signal == "\xFF" then
				break 
			end
			
			-- get key by starting at cursor, breaking out when we see a null terminator
			local key = ""
			
			for i = byref.cursor, #bytes do
				local c = bytes:sub( i, i )
				if c == "\x00" then
					byref.cursor = i
					break
				end
				key = key..c
			end
			byref.cursor = byref.cursor + 1
			
			-- cursor should be pointed at data now
			-- derive value
			local value
			if signal == "\x00" then
				-- type boolean
				value = bytes:sub( byref.cursor, byref.cursor ) 
				value = ( value == '\x01' )
				byref.cursor = byref.cursor + 1
			elseif signal == "\x01" or signal == "\x02" then
				-- type number or string
				value = ""
				for i = byref.cursor, #bytes do
					local c = bytes:sub( i, i )
					if c == "\x00" then
						byref.cursor = i
						break
					end
					value = value..c
				end
				byref.cursor = byref.cursor + 1

				-- if we got here from a number, convert it to number type. else, leave it go!
				if signal == "\x01" then
					value = tonumber( value )
				end
			elseif signal == "\x03" then
				-- type table
				-- nice, easy: repeat this entire process using a recursive call
				value = _bblib.deserialise( bytes, byref )
			end
			
			-- set table key to value
			tbl[key] = value
			
		end
		
		return tbl
	end,
	
	get_cid = function( self ) 
		self.lastcid = self.lastcid + 1
		return "bb"..self.lastcid
	end,
	
	instantiate_pop = function( id, pop, entity_type )	
		local bbobject = _classes[ bluebear.Constants.LOT_ENTITY_TYPES[ entity_type ] ][ id ].new()
		bbobject.type = id 
		bbobject._cid = _bblib:get_cid()
		
		-- Every object has a poptable, if anything just to contain the _sys table
		local poptable = _bblib.deserialise( pop )
		bbobject._sys = poptable._sys

		if type( bbobject.marshal ) == "function" then
			bbobject:unmarshal( poptable )
		end
		
		print( "instantiated lot bbobject ("..bbobject._cid.."): "..bbobject.catalog.name )
		
		return bbobject
	end

};

-- testing some new shit for the object model
local Object = {
	
	a = 1,
	
	c = {
		b = 2
	}
	
}

local Object1 = {
	
	b = 2
	
}

local Object2 = {
	
	a = 3
	
}

setmetatable( Object, { __index = Object } )
setmetatable( Object1, { __index = Object } )
setmetatable( Object2, { __index = Object1 } )
