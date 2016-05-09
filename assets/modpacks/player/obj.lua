--[[
  Defines the standard player that you can put into a house and control.

  High level object, try not to import too much shit as you'll start getting
  circular dependencies.
]]

bluebear.engine.require_modpack( 'motives' )

local Player = bluebear.extend( 'system.doll.base', 'game.doll.player' )

bluebear.register_class( Player )
