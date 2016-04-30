--[[
  Base motives (game.motive.*) for the basic BlueBear game
--]]

-- Hunger motive group
local HUNGER_MOTIVE_GROUP = "Hunger"

local Food = bluebear.extend( "system.motive.base", {
  motive_name = "Food",
  motive_group = HUNGER_MOTIVE_GROUP
} )

local Drink = bluebear.extend( "system.motive.base", {
  motive_name = "Drink",
  motive_group = HUNGER_MOTIVE_GROUP
} )

bluebear.register_motive( "game.motive.hunger.food", Food )
bluebear.register_motive( "game.motive.hunger.drink", Drink )
