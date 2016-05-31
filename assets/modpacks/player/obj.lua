--[[
  Defines the standard player that you can put into a house and control.

  High level object, try not to import too much shit as you'll start getting
  circular dependencies.
]]

bluebear.engine.require_modpack( 'motives' )

local Player = bluebear.extend( 'system.doll.base', 'game.doll.player' )

-- debug stuff
function Player:decay_my_motives()
  print( Player.name, 'Hello from Lua! I am doll ('..self._cid..') and I am decaying my motives!' )

  bluebear.get_class( 'system.doll.base' ).decay_my_motives( self )
end

bluebear.register_class( Player )
