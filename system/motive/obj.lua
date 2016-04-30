--[[
  Applies methods to the "bluebear" global that concern the registration of motives, and defines a few
  basic motives.
--]]

bluebear.motives = {}

bluebear.register_motive = function( motiveName, motiveObject )
  bluebear.motives[ motiveName ] = motiveObject
end
