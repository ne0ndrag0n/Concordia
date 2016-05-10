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
  Populate the doll with its motives
--]]
function Doll:create_motives()
  -- Get list of motives, all classes deriving frmo system.motive.base
  -- USE cached list if provided, this is an expensive operation
  local motive_classes = self:get_usable_motives()

  local motives = {}
  for index, Class in ipairs( motive_classes ) do
    local motive_instance = Class:new( self )

    if motives[ motive_instance.motive_group ] == nil then motives[ motive_instance.motive_group ] = {} end

    motives[ motive_instance.motive_group ][ Class.name ] = motive_instance
  end

  self.motives = motives
end

--[[
  Override this function to control which motives are visible to your doll class.
--]]
function Doll:get_usable_motives()
  motives_list = motives_list or bluebear.get_classes_of_type( 'system.motive.base' )

  return motives_list
end

function Doll:initialize()
  self:create_motives()
end

--[[
  Kick off the virtual thread that decays this doll's motives.
--]]
function Doll:on_create()
  self:defer():then_call( 'decay_my_motives' )
end

--[[
  This function is designed to run in a loop and decay all motives once per game minute.
--]]
function Doll:decay_my_motives()

  for group_name, group in pairs( self.motives ) do
    for class_id, motive in pairs( group ) do
      motive:decay()
    end
  end

  self:sleep( bluebear.util.time.minutes_to_ticks( 1 ) ):then_call( 'decay_my_motives' )
end

--[[
  Play the death animation, display a death dialog box, remove the doll from the lot,
  and replace it with an urn entity.
--]]
function Doll:die()
  print( "Doll died!" )
  -- TODO
end

bluebear.register_class( Doll )
