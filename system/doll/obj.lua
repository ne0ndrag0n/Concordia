--[[
  Defines the Doll entity for BlueBear (system.doll.base : system.object.base)

  Requires the motives system modpack
--]]

bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "object" )
bluebear.engine.require_modpack( "motive" )

local Doll = bluebear.extend( "system.object.base", "system.doll.base", {
  motives = {}
} )

bluebear.register_class( Doll )
