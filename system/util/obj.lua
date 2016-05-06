--[[
  Set up the "util" Underscore-like functionality available in BlueBear
--]]

bluebear.util = {
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
  end,

  time = {
    minutes_to_ticks = function( minutes )
      return bluebear.engine.tick_rate * minutes
    end,

    seconds_to_ticks = function( seconds )
      return bluebear.util.time.minutes_to_ticks( seconds / 60 )
    end
  }
}
