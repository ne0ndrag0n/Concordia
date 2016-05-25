bluebear.engine.require_modpack( "middleclass" )
bluebear.engine.require_modpack( "class" )

--[[
  A stemcell is a type of object that can wrap one of several predefined C++ object types, enabling you to
  instantiate an object normally only accessible in native code within the Luasphere. They are named because
  instances mimic behaviour of a stem cell, which begins as a "blank slate" and assumes the form of a particular
  type of cell. When setting up a stemcell, self._inst is populated with userdata representing the C++ object
  this instance wraps, and functions are overlaid on top of the instance.
--]]

local Stemcell = class( 'system.stemcell' )

Stemcell.TYPES = {
  EVENT_MANAGER = 'event-manager'
}

function Stemcell:initialize( type )
  self._inst = nil

  bluebear.engine.setup_stemcell( self, type )
end

bluebear.register_class( Stemcell )
