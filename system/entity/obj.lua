bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "object" )
bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "promise" )

local Entity = bluebear.extend( 'system.object.base', 'system.entity.base' )

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
end

--[[
	Sleep numTicks amount of ticks and then call the function on this object named by functionName
--]]
function Entity:sleep( numTicks )
	local ticks = numTicks + bluebear.engine.current_tick

	-- do nothing but return a new promise with the ticks set to "ticks"
	return bluebear.get_class( 'system.promise.base' ):new( self, ticks )
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

	table.insert( ticks_table, { method = method, arguments = wrapped_arguments } )
end

bluebear.register_class( Entity )
