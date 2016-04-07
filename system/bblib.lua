function string:split(sep)
	local sep, fields = sep or ":", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

_bblib = {
	lastcid = 0,

	extend_object = function( parent, subclass, deep_tables )
		local new_table = {}

		-- Shallow (or deep) copy parent to new_table
		for k,v in pairs( parent ) do
			if deep_tables == true and type( v ) == "table" then
				new_table[ k ] = _bblib.deep_copy_table( v )
			else
				new_table[ k ] = v
			end
		end

		-- Overwrite with shallow references to subclass
		for k,v in pairs( subclass ) do
			if deep_tables == true and type( v ) == "table" then
				new_table[ k ] = v
			else
				new_table[ k ] = v
			end
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

	get_cid = function( self )
		self.lastcid = self.lastcid + 1
		return "bb"..self.lastcid
	end

};
