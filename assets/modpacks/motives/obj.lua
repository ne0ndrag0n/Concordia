--[[
  Base motives (game.motive.*) for the basic BlueBear game
--]]

-- Hunger motive group
local HUNGER_MOTIVE_GROUP = "Hunger"

local Food = bluebear.extend( "system.motive.base", {
  motive_name = "Food",
  motive_group = HUNGER_MOTIVE_GROUP,

  decay = function( self )
    -- If doll has a high "active" motive, decrement faster
    self.super:decay()
  end
} )

bluebear.register_motive( "game.motive.food", Food )
