function string:split(sep)
	local sep, fields = sep or ":", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

_bblib = {
	lastcid = 0,

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
	
	clear_objects = function() 
		_lotinsts = {};
	end,
	
	get_cid = function( self ) 
		self.lastcid = self.lastcid + 1
		return "bb"..self.lastcid
	end,
	
	instantiate_pop = function( id, pop ) 
		-- Testing for now
		_bblib.clear_objects()
		
		local bbobject = _bbobjects[ id ].new()
		bbobject._cid = _bblib:get_cid()
		
		-- Every object has a poptable, if anything just to contain the _sys table
		local poptable = _bblib.deserialise( pop )
		bbobject._sys = poptable._sys
		
		if type( bbobject.marshal ) == "function" then
			bbobject:unmarshal( poptable )
		end
		
		_lotinsts[ bbobject._cid ] = bbobject
		print( "instantiated lot bbobject ("..bbobject._cid.."): "..bbobject.catalog.name )
		
		return _lotinsts[ bbobject._cid ]
	end

};
