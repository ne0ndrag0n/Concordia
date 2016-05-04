--[[
  Defines the Doll entity for BlueBear (system.doll.base : system.object.base)

  Requires the motives system modpack
--]]

bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "entity" )

local Doll = bluebear.extend( "system.entity.base", "system.doll.base", {
  motives = {}
} )

bluebear.register_class( Doll )
