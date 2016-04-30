--[[
  Applies methods to the "bluebear" global that concern the registration of motives, and defines a few
  basic motives.
--]]

bluebear.engine.require_modpack( "yaci" )
bluebear.engine.require_modpack( "class" )

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

-- Convience methods that are set up on the BlueBear object
-- Set up an array of class names - this saves time when constructing Dolls
-- USE bluebear.register_motive WHEN REGISTERING MOTIVE CLASSES SO YOUR DOLLS
-- CAN USE THE MOTIVES!
bluebear.motives = {}
bluebear.register_motive = function( class_name, motive )
  -- Register the motive as an ordinary class
  bluebear.register_class( class_name, motive )

  -- Save the class name on bluebear.motives array: This is a simple optimisation
  -- that allows Doll objects to construct quickly (they need to create instances
  -- of each registered motive)
  table.insert( bluebear.motives, class_name )
end
