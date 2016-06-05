-- Hunger motive group
local HUNGER_MOTIVE_GROUP = "Hunger"

local Food = bluebear.extend( "system.motive.base", "game.motive.hunger.food", {
  motive_name = "Food",
  motive_group = HUNGER_MOTIVE_GROUP,

  --[[
    Subtract by this value when decaying motive.
  --]]
  decay_rate = 6,

  --[[
    The doll begins starving when the motive value decreases under this level.
  --]]
  starvation_threshold = 10,

  --[[
    This is the tick level the Doll first started starving at.
  --]]
  first_starving_at = nil,

  --[[
    Dolls die if below self.starvation_threshold for greater than 3 day games.
    3 days in hours is 72 hours, use bluebear.util.time.hours_to_ticks to get
    the number of ticks in 72 game hours (1 game minute = 1 real life second)
  --]]
  time_to_starve = bluebear.util.time.hours_to_ticks( 72 )
} )

function Food:load( data )
  bluebear.get_class( 'system.motive.base' ).load( self, data )

  if type( data.decay_rate ) == 'number' then self.decay_rate = data.decay_rate end
  if type( data.starvation_threshold ) == 'number' then self.starvation_threshold = data.starvation_threshold end
  if type( data.first_starving_at ) == 'number' then self.first_starving_at = data.first_starving_at end
end

--[[
  Describes how the Food motive decays:

  * Decay hunger six points a game hour. The motive will be decayed every game
    minute, or 60 times per game hour. Every 10 game minutes, decay hunger.
  * After each decay, check to see that the doll has not starved to death. The
    doll starves to death after the motive spends three game days under the
    starvation_threshold.
--]]
function Food:decay()
  if self.decay_calls >= 10 then
    self.decay_calls = 0

    bluebear.get_class( 'system.motive.base' ).decay( self )

    self:check_starve()
  else
    self.decay_calls = self.decay_calls + 1
  end
end

--[[
  Check to see if the doll needs to die from starvation. The doll starves to death
  if the Food motive remains under 10 for three straight game days.
--]]
function Food:check_starve()
  if self.value <= self.starvation_threshold then
    -- We are starving!
    if self.first_starved_at == nil then
      -- We weren't starving the last time this happened: start the clock
      self.first_starved_at = bluebear.engine.current_tick
    else
      -- We're still starving
      if bluebear.engine.current_tick >= self.first_starving_at + self.time_to_starve then
        -- Doll dies
        -- TODO: Doll has kill() method which accepts death animation, use system.doll.base.ANIMATIONS
        -- table with crawl-and-starve-to-death animation
        self.doll:kill( self.doll.ANIMATIONS.STARVE )
      end
    end
  else
    -- We are not starving, or aren't starving anymore
    -- Make sure self.first_starved_at gets reset to nil
    self.first_starved_at = nil
  end
end

--[[
  Gets the importance of the Food motive. Food is more important the closer it is
  to zero. If the motive is zero, it is full importance. If the motive is full,
  the motive is not particularly important (as you just ate!).

  Motive importance has an impact on a doll's greater mood. Food has a dramatic
  impact on mood as it is lesser and lesser.
--]]
function Food:get_importance()
  local percentage = self.value / 100

  -- Check the graph of (1/32)^x to see how Food is vastly more important as it is less
  return 0.03125^percentage
end
bluebear.register_class( Food )
