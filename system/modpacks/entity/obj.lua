bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "promise" )
bluebear.engine.require_modpack( "stemcell" )

local Entity = class( 'system.entity.base' )
local Stemcell = bluebear.get_class( "system.stemcell" )
local TimedPromise = bluebear.get_class( 'system.promise.timer' )

--[[
	Sleep numTicks amount of ticks and then call the function on this object named by functionName
--]]
function Entity:sleep( numTicks )
	-- do nothing but return a new promise with the ticks set to "numTicks"
	return TimedPromise:new( numTicks )
end

--[[
  Gets a curated list of interactions visible to the passed-in player.

  Probably something we can do in C++ during the Picasso milestone. This will
  have to stay in Lua, if we want NPC dolls to call on objects to list their
  interactions.
--]]
function Entity:get_interactions( player )
  local eligible_interactions = {}

  for index, interaction in ipairs( self.interactions ) do
    if interaction.condition( player, self ) then
      table.insert( eligible_interactions, interaction )
    end
  end

  return eligible_interactions
end

--[[
  Change the sprite(s) of this entity. tile_x and tile_y select a tile
  relative to the object's origin tile (upper left relative to rotation state 0)
--]]
function Entity:change_graphic( tile_x, tile_y, graphic )
  -- TODO: Change the graphic of this entity
end

--[[
  Unroll the animation sequence across _sys._sched as a series of change_graphic calls
  Return a promise pre-computed for the duration of this animation sequence
--]]
function Entity:animate( sequence )
  -- TODO: Unroll the animation and compute its total duration
  local animation_ticks = bluebear.engine.current_tick + 1

  return bluebear.get_class( 'system.promise.timer' ):new( self, animation_ticks )
end

--[[
  Provide interfaces for objects placed on a lot
--]]
function Entity:on_create() end
function Entity:on_destroy() end

bluebear.register_class( Entity )
