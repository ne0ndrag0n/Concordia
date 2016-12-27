--[[
  Set up the "util" Underscore-like functionality available in BlueBear
--]]

math.randomseed(os.time())

bluebear.util.lastcid = 0

bluebear.util.extend = function( destination, ... )
  local arg = { ... }

  -- Overwrite with shallow references to each source
  for index, source in ipairs( arg ) do
    for key, value in pairs( source ) do
      destination[ key ] = value
    end
  end

  return destination
end

-- Courtesy lua-users wiki: Copy Table - http://lua-users.org/wiki/CopyTable
bluebear.util.deep_copy = function( orig )
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
end

bluebear.util.concatenate_arrays = function( first, last )
  local new_table = {}

  for k,v in pairs( first ) do
    table.insert( new_table, v )
  end

  for k,v in pairs( last ) do
    table.insert( new_table, v )
  end

  return new_table
end

bluebear.util.get_cid = function()
  bluebear.util.lastcid = bluebear.util.lastcid + 1
  return "bb"..bluebear.util.lastcid
end

bluebear.util.split = function( str, delimiter )
  local delimiter, fields = delimiter or ":", {}
  local pattern = string.format("([^%s]+)", delimiter)
  str:gsub(pattern, function(c) fields[#fields+1] = c end)
  return fields
end

bluebear.util.round = function(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end

--[[
  Gets the index of "needle" in the "collection". If the needle is not
  in the collection, return 0.
--]]
bluebear.util.array_index_of = function( collection, needle )
  for index, value in ipairs( collection ) do
    if needle == value then
      return index
    end
  end

  return 0
end

bluebear.util.time = {
  hours_to_ticks = function( hours )
    return bluebear.util.time.minutes_to_ticks( hours * 60 )
  end,

  minutes_to_ticks = function( minutes )
    return bluebear.util.round( bluebear.engine.tick_rate * minutes )
  end,

  seconds_to_ticks = function( seconds )
    return bluebear.util.time.minutes_to_ticks( seconds / 60 )
  end
}

bluebear.util.print_upvalues = function( func )
  local idx = 1
  while true do
    local name, val = debug.getupvalue(func, idx)
    if not name then break end
    print( "bluebear.util.print_upvalues", "[Upvalue "..tostring( idx ).."] "..name..", "..tostring( val ) )
    idx = idx + 1
  end
end

bluebear.util.get_upvalue_by_name = function( func, name )
  local i = 1
  while true do
    local n, v = debug.getupvalue(func, i)
    if not n then break end
    if n == name then return v end
    i = i + 1
  end
end
