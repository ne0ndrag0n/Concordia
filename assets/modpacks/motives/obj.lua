--[[
  Base motives (game.motive.*) for the basic BlueBear game
--]]

-- Hunger motive group
local HUNGER_MOTIVE_GROUP = "Hunger"

local Food = bluebear.extend( "system.motive.base", {
  motive_name = "Food",
  motive_group = HUNGER_MOTIVE_GROUP
} )

local Motive = bluebear.get_class( "system.motive.base" )
print( Motive.static.motive_name )
--bluebear.register_motive( "game.motive.food", Food )
