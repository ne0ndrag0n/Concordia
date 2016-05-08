--[[
  Applies methods to the "bluebear" global that concern the registration of motives, and defines a few
  basic motives.
--]]

bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "object" )

local Motive = bluebear.extend( 'system.object.base', 'system.motive.base' )

Motive.motive_name = "Root Motive"
Motive.motive_group = "Motive Group"
Motive.decay_calls = 0
Motive.decay_rate = 1

function Motive:initialize( doll )
  self.doll = doll
  self.value = 50
end

function Motive:decay()
  if self.value > 0 then
    self.value = self.value - self.decay_rate
  end
end

function Motive:increment_by( value )
  self:set_value( self.value + value )
end

function Motive:decrement_by( value )
  self:set_value( self.value - value )
end

function Motive:set_value( value )
  if value > 100 then
    self.value = 100
  elseif value < 0 then
    self.value = 0
  else
    self.value = value
  end
end

bluebear.register_class( Motive )
