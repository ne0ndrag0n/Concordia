function string:split(sep)
	local sep, fields = sep or ":", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

_bblib = {
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
	            copy[_bblib.deep_copy(orig_key)] = _bblib.deep_copy(orig_value)
	        end
	        setmetatable(copy, _bblib.deep_copy(getmetatable(orig)))
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
		_bblib.lastcid = _bblib.lastcid + 1
		return "bb".._bblib.lastcid
	end

};
