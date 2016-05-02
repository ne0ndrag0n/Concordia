--[[
  Base motives (game.motive.*) for the basic BlueBear game
--]]

-- Hunger motive group
local HUNGER_MOTIVE_GROUP = "Hunger"

local Food = bluebear.extend( "system.motive.base", {
  motive_name = "Food",
  motive_group = HUNGER_MOTIVE_GROUP,
  decay_rate = 6,

  first_starving_at = nil,

  starvation_threshold = 10,

  --[[
    Describes how the Food motive decays:

    * Decay hunger six points an hour. Motive decay is called every 30 ticks,
      and 30 ticks equal one game minute. There are 1800 ticks in a game hour.
      Therefore, every 10 times decay is called, subtract hunger by 6.
    * After each decay, check to see that the doll has not starved to death. The
      doll starves to death after the motive spends three game days under the
      starvation_threshold.
  --]]
  decay = function( self )
    if self.decay_calls >= 10 then
      self.decay_calls = 0

	  bluebear.get_class( 'system.motive.base' ).decay( self )
      self:check_starve()
    else
      self.decay_calls = self.decay_calls + 1
    end
  end,

  --[[
    Check to see if the doll needs to die from starvation. The doll starves to death
    if the Food motive remains under 10 for three straight game days. There are 43200
    ticks in a game day at 30 ticks per real-life second. Doll dies at
    self.first_starving_at + 129600 ticks, unless the starvation threshold is not met.
  --]]
  check_starve = function( self )
    if self.value <= self.starvation_threshold then
      -- We are starving!
      if self.first_starved_at == nil then
        -- We weren't starving the last time this happened: start the clock
        self.first_starved_at = bluebear.engine.current_tick
      else
        -- We're still starving - check if bluebear.engine.current_tick
        -- is greater than or equal to self.first_starving_at + 129600
        if bluebear.engine.current_tick >= self.first_starving_at + 129600 then
          -- Doll dies
          -- TODO: Doll has die() method which accepts death animation, use system.doll.base.ANIMATIONS
          -- table with crawl-and-starve-to-death animation
          self.doll:die( self.doll.ANIMATIONS.STARVE )
        end
      end
    else
      -- We are not starving, or aren't starving anymore
      -- Make sure self.first_starved_at gets reset to nil
      self.first_starved_at = nil
    end
  end
} )

bluebear.register_motive( "game.motive.hunger.food", Food )
