_bblib = {
	serialise = function( table, content ) 
		-- TODO
	end,
	
	deserialise = function( bytes ) 
		local tbl = {}
		local cursor = 1
		
		while true do
			-- get character at cursor position, then advance cursor to key
			local signal = bytes:sub( cursor, cursor )
			cursor = cursor + 1
			
			-- if signal is \xFF, then break
			if signal == "\xFF" then break end
			
			-- get key by starting at cursor, breaking out when we see a null terminator
			local key = ""
			
			for i = cursor, #bytes do
				local c = bytes:sub( i, i )
				if c == "\x00" then
					cursor = i
					break
				end
				key = key..c
			end
			cursor = cursor + 1
			
			-- cursor should be pointed at data now
			-- derive value
			local value
			if signal == "\x00" then
				-- type boolean
				value = bytes:sub( cursor, cursor ) 
				value = ( value == '\x01' )
				cursor = cursor + 1
			elseif signal == "\x01" or signal == "\x02" then
				-- type number or string
				value = ""
				for i = cursor, #bytes do
					local c = bytes:sub( i, i )
					if c == "\x00" then
						cursor = i
						break
					end
					value = value..c
				end
				cursor = cursor + 1

				-- if we got here from a number, convert it to number type. else, leave it go!
				if signal == "\x01" then
					value = tonumber( value )
				end
			elseif signal == "\x03" then
				-- type table
				-- nice, easy: repeat this entire process using a recursive call
				value = _bblib.deserialise( bytes:sub( cursor, #bytes ) )
			end
			
			-- set table key to value
			tbl[key] = value
			
		end
		
		return tbl
	end
};
