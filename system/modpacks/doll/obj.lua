--[[
  Defines the Doll entity for BlueBear (system.doll.base : system.entity.base)

  Requires the motives system modpack
--]]

bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "entity" )
bluebear.engine.require_modpack( "motive" )
bluebear.engine.require_modpack( "util" )

local motives_list = nil
local STATES = {
  -- The doll is not currently doing anything interaction-related
  IDLE = 'idle',
  -- The doll is in the process of prearing for the interaction at the top of the queue
  PREPARING = 'preparing',
  -- The doll is currently engaged in the interaction at the top of the queue
  INTERACTING = 'interacting',
  -- The doll is wrapping up its interaction
  CONCLUDING = 'concluding',
  -- The player chose to abort this interaction early, and we need to jump to CONCLUDING state early
  ABORTING = 'aborting'
}

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
  no_decay = false,

  --[[
    The current state of the doll
  --]]
  current_state = nil,

  --[[
    This table holds all interactions due for the doll. The interactions are processed first-in,
    first-out. Therefore, the element at the top of this table (1), # > 0, is
    the one that is always processed.
  --]]
  interaction_queue = nil,

  --[[
    These are the five states your doll can be in at any given time.
  --]]
  STATES = STATES,

  --[[
    Table containing functions of state transitions
  --]]
  STATE_TRANSITIONS = {
    [STATES.PREPARING] = 'prepare',
    [STATES.INTERACTING] = 'interact',
    [STATES.CONCLUDING] = 'conclude',
    [STATES.ABORTING] = 'abort'
  },

  --[[
    Number of ticks a doll should wait between checking for new interactions
  --]]
  HEARTBEAT_INTERVAL = 15,

  --[[
    TODO: Placeholder for BlueBear Picasso milestone
  --]]
  ANIMATIONS = {}

} )

--[[
  Load a doll entity
--]]
function Doll:load( data )
  bluebear.get_class( 'system.entity.base' ).load( self, data )

  -- Load motives
  for class_name, serial_instance in pairs( data.saved_motives ) do
    -- Deserialise the nested motive data
    self.motives[ class_name ]:load( serial_instance )
  end
end

--[[
  Populate the doll with its motives
--]]
function Doll:create_motives()
  -- Get list of motives, all classes deriving frmo system.motive.base
  -- USE cached list if provided, this is an expensive operation
  local motive_classes = self:get_usable_motives()

  local motives = {}
  for index, Class in ipairs( motive_classes ) do
    motives[ Class.name ] = Class:new( self )
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

--[[
  Retrieve the motive class given by motive_id and change its value by "value". Only perform
  this action if the given motive exists on this type of doll; if it doesn't, there is nothing
  to adjust.
--]]
function Doll:update_motive( motive_id, value )
  print( Doll.name, "Updating motive "..motive_id )
  local motive = self.motives[ motive_id ]

  if motive then
    self.motives[ motive_id ]:add_to_value( value )
  end
end

function Doll:initialize()
  bluebear.get_class( 'system.entity.base' ).initialize( self )

  self.interaction_queue = {}
  self:create_motives()
  self:change_state( Doll.STATES.IDLE )
end

--[[
  Change the state of the doll. If necessary, call functions on interactions that are due to be called
  when a state transition is triggered.
--]]
function Doll:change_state( new_state )
  self.current_state = new_state

  -- idle is a special case: when transitioning to idle, the assumption is that
  -- there is a previous interaction which needs to be removed from the queue
  -- Therefore, remove it
  if new_state == Doll.STATES.IDLE then
    -- Remove the element furthest in the queue
    print( Doll.name, "Removing interaction "..tostring( self.interaction_queue[1] ) )
    table.remove( self.interaction_queue, 1 )
  else
    local state_transition = Doll.STATE_TRANSITIONS[ new_state ]

    if state_transition then
      local queue_top = self.interaction_queue[1]
      queue_top.interaction[ state_transition ]( self, queue_top.entity )
    end
  end
end

--[[
  Copy the interaction object and add it to this doll's interaction queue.
--]]
function Doll:enqueue_interaction( entity, interaction )
  -- push a queued_interaction, which is a container table holding
  -- the object instance and interaction
  local queued_interaction = {
    entity = entity,
    interaction = interaction
  }

  print( Doll.name, "Enqueueing interaction table "..tostring( queued_interaction ) )
  table.insert( self.interaction_queue, queued_interaction )
end

--[[
  Return a system.promise.pathfinder
--]]
function Doll:walk_to( object )
  return bluebear.get_class( 'system.promise.pathfinder' ):new( self, object )
end

--[[
  Kick off the virtual thread that decays this doll's motives.
--]]
function Doll:on_create()
  self:defer():then_call( 'main' )
  self:defer():then_call( 'decay_my_motives' )
end

--[[
  This vthread handles interactions that appear in the interaction queue
--]]
function Doll:main()
  -- check the interaction queue if we are in IDLE state
  if self.current_state == Doll.STATES.IDLE then
    if #self.interaction_queue > 0 then
      -- take next item from queue and process it
      self:change_state( Doll.STATES.PREPARING )
    else
      -- TODO for ET3, the main() vthread should look around its environment
      -- for things that satisfy its needs if it has nothing to do
      -- Account for waiting (the doll should be able to just stand there for a bit)
    end
  end
  self:sleep( Doll.HEARTBEAT_INTERVAL ):then_call( 'main' )
end

--[[
  This function is designed to run in a loop and decay all motives once per game minute.
--]]
function Doll:decay_my_motives()

  for class_id, motive in pairs( self.motives ) do
      motive:decay()
  end

  self:sleep( bluebear.util.time.minutes_to_ticks( 1 ) ):then_call( 'decay_my_motives' )
end

--[[
  Play the death animation, display a death dialog box, remove the doll from the lot,
  and replace it with an urn entity.
--]]
function Doll:kill()
  print( Doll.name, "Doll died!" )
  -- TODO
end

bluebear.register_class( Doll )
