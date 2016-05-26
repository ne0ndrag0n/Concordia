bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "object" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "promise" )
bluebear.engine.require_modpack( "stemcell" )

local Entity = bluebear.extend( 'system.object.base', 'system.entity.base' )
local Stemcell = bluebear.get_class( "system.stemcell" )

function Entity:load( saved )
  bluebear.get_class( 'system.object.base' ).load( self, saved )

  local data = saved or {}

  self._sys = data._sys
end

function Entity:save()
	local out = bluebear.get_class( 'system.object.base' ).save( self )

  out._sys = self._sys

	-- When serialising, iterate through the _sys._sched table in out and
	-- replace table references to anything containing a bbXXX _cid with
	-- the serial format

	-- jesus christ what a fright
	for time, callbacks in pairs( out._sys._sched ) do
		for index, callback in ipairs( callbacks ) do
			for argumentIndex, argument in ipairs( callback.arguments ) do
				-- If type of argument is a table, check if it has a _cid property
				-- If it does, this argument will be replaced by the serialised version
				if type( argument ) == "table" and string.match( argument._cid, "bb%d" ) then
					callback.arguments[ argumentIndex ] = "t/"..argument._cid
				end
			end
		end
	end

	return out
end

--[[
  Set up a system.entity.base. This object ALWAYS, ALWAYS has a _sys table, containing
  at least the sub-table _sched.
]]
function Entity:initialize()
  self._sys = {
    _sched = {}
  }

  self.event_manager = Stemcell:new( Stemcell.TYPES.EVENT_MANAGER )
end

--[[
	Sleep numTicks amount of ticks and then call the function on this object named by functionName
--]]
function Entity:sleep( numTicks )
	local ticks = numTicks + bluebear.engine.current_tick

	-- do nothing but return a new promise with the ticks set to "ticks"
	return bluebear.get_class( 'system.promise.timer' ):new( self, ticks )
end

--[[
  Simple defer (schedule this for the next available tick, without increment)
--]]
function Entity:defer()
  return bluebear.get_class( 'system.promise.base' ):new( self )
end

--[[
	Enter in a callback on this object's _sys._sched table for the destination tick
--]]
function Entity:register_callback( tick, method, wrapped_arguments )
	local ticks_key = tostring( tick )
	local ticks_table

	if self._sys._sched[ ticks_key ] == nil then
		self._sys._sched[ ticks_key ] = {}
	end

	ticks_table = self._sys._sched[ ticks_key ]

  local descriptor = {
    method = method,
    arguments = wrapped_arguments
  }

	table.insert( ticks_table, descriptor )

  return descriptor
end

--[[
  Gets a curated list of interactions visible to the passed-in player.

  Probably something we can do in C++ during the Picasso milestone. This will
  have to stay in Lua, if we want NPC dolls to call on objects to list their
  interactions.
--]]
function Entity:get_interactions( player )
  local eligible_interactions = {}

  for index, interaction in ipairs( self.interactions ) do
    if interaction.condition( player, self ) then
      table.insert( eligible_interactions, interaction )
    end
  end

  return eligible_interactions
end

--[[
  Given a cancelable, cancel an upcoming callback before it happens.
--]]
function Entity:cancel_callback( cancelable )
  -- Find the callback using the information provided in the cancelable
  local array = self._sys._sched[ cancelable.tick ]

  table.remove( array, bluebear.util.array_index_of( array, cancelable.callback ) )
end

--[[
  Change the sprite(s) of this entity. tile_x and tile_y select a tile
  relative to the object's origin tile (upper left relative to rotation state 0)
--]]
function Entity:change_graphic( tile_x, tile_y, graphic )
  -- TODO: Change the graphic of this entity
end

--[[
  Unroll the animation sequence across _sys._sched as a series of change_graphic calls
  Return a promise pre-computed for the duration of this animation sequence
--]]
function Entity:animate( sequence )
  -- TODO: Unroll the animation and compute its total duration
  local animation_ticks = bluebear.engine.current_tick + 1

  return bluebear.get_class( 'system.promise.timer' ):new( self, animation_ticks )
end

--[[
  Provide interfaces for objects placed on a lot
--]]
function Entity:on_create()
  -- This function will be called when an Entity is created and placed on a lot. Load will not be called.
  -- Assign a generated cid.
  self._cid = bluebear.util.get_cid()
end
function Entity:on_destroy() end

bluebear.register_class( Entity )
