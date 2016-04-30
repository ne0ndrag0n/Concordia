--[[
  Applies methods to the "bluebear" global that concern the registration of motives, and defines a few
  basic motives.
--]]

bluebear.engine.require_modpack( "yaci" )
bluebear.engine.require_modpack( "class" )

local Motive = newclass()

Motive.motive_name = "Root Motive"
Motive.motive_group = "Motive Group"

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

-- Convience methods that are set up on the BlueBear object
bluebear.motives = {}
bluebear.register_motive = function( class_name, Motive )
  -- Register the motive as an ordinary class
  bluebear.register_class( class_name, Motive )

  -- Make sure that an array exists in bluebear.motives for the motive's group
  if bluebear.motives[ Motive.motive_group ] == nil then
    bluebear.motives[ Motive.motive_group ] = {}
  end

  -- Insert this class name into that table. On init, dolls create a table full
  -- of keys that represent the motive groups, pointing to tables holding the
  -- submotives for that group. Each of *those* tables has keys as the given
  -- name of the motive, and the values as new motive instances.
  table.insert( bluebear.motives[ Motive.motive_group ], class_name )
end
