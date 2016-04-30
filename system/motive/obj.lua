--[[
  Applies methods to the "bluebear" global that concern the registration of motives, and defines a few
  basic motives.
--]]

bluebear.engine.require_modpack( "yaci" )

local Motive = newclass()

Motive.name = "Root Motive"

function Motive:init( doll )
  self.doll = doll
  self.value = 50
end

function Motive:decay()
  if self.value > 0 then
    self.value = self.value - 1
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

bluebear.register_class( "system.motive.base", Motive )
