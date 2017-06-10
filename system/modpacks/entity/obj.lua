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
  Provide interfaces for objects placed on a lot
--]]
function Entity:on_create() end
function Entity:on_destroy() end

bluebear.register_class( Entity )
