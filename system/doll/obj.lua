--[[
  Defines the Doll entity for BlueBear (system.doll.base : system.object.base)

  Requires the motives system modpack
--]]

bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "entity" )
bluebear.engine.require_modpack( "motive" )

local motives_list = nil

local Doll = bluebear.extend( "system.entity.base", "system.doll.base", {

  --[[
    Holds your motives table, which needs to be created at instantiation as its own copy
    (Recall that in Lua, all tables are instantiated by reference)
  --]]
  motives = nil,

  --[[
    Your subclasses may change this switch to prevent the decay of motives. This is useful
    for NPCs which must "live forever" in some scenarios (e.g. maid or pizza boy shouldn't
    be able to die). This may be changed on runtime as well.
  --]]
  no_decay = false

} )

--[[
  Populate the doll with the same motives all dolls have.
--]]
function Doll:create_motives()
  -- Get list of motives, all classes deriving frmo system.motive.base
  -- USE cached list if provided, this is an expensive operation
  motives_list = motives_list or bluebear.get_classes_of_type( 'system.motive.base' )
end

function Doll:initialize()
  self:create_motives()
end

bluebear.register_class( Doll )
