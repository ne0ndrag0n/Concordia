--[[
	BlueBear base object
	This object is the object that all other Luasphere objects descend from - the base object
	Anything that can be placed on a lot and ran as an entity is an "object"
--]]

bluebear.engine.require_modpack( "middleclass" )
bluebear.engine.require_modpack( "class" )
bluebear.engine.require_modpack( "promise" )

local Object = class( 'system.object.base' )

function Object:load( saved )
	local data = saved or {}

	self._cid = data._cid
	self._sys = data._sys
end

function Object:save()
	local out = {
		_sys = self._sys,
		_cid = self._cid
	}

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

function Object:setup()
	-- abstract - This is the "real" constructor of objects
end

--[[
	Sleep numTicks amount of ticks and then call the function on this object named by functionName
--]]
function Object:sleep( numTicks )
	local ticks = numTicks + bluebear.engine.current_tick

	-- do nothing but return a new promise with the ticks set to "ticks"
	return bluebear.classes.system.promise.base( self, ticks )
end

--[[
	Enter in a callback on this object's _sys._sched table for the destination tick
--]]
function Object:register_callback( tick, method, wrapped_arguments )
	local ticks_key = tostring( tick )
	local ticks_table

	if self._sys._sched[ ticks_key ] == nil then
		self._sys._sched[ ticks_key ] = {}
	end

	ticks_table = self._sys._sched[ ticks_key ]

	table.insert( ticks_table, { method = method, arguments = wrapped_arguments } )
end

-- Private methods (do not override these!)
-- Called by the engine, runs all callbacks due for the given ticks
function Object:_run()
	-- Is there a _sys._sched entry for currentTick?
	local currentTick = tostring( bluebear.engine.current_tick )
	local callbackList = self._sys._sched[ currentTick ]

	if type( callbackList ) == "table" then
		-- Clear the callback table from self._sys._sched
		self._sys._sched[ currentTick ] = nil

		-- Fire each callback in the table
		for index, callbackTable in ipairs( callbackList ) do
			self[ callbackTable.method ]( self, table.unpack( callbackTable.arguments ) )
		end
	end

end

--[[
	This function is only to be called on each object after all objects are finished loading. Basically,
	deserialize all references to bluebear objects which take the form "t/"..self._cid
--]]
function Object:_deserialize_function_refs()
	for time, callbacks in pairs( self._sys._sched ) do
		for index, callback in ipairs( callbacks ) do
			for argumentIndex, argument in ipairs( callback.arguments ) do
				-- if this argment is a string and it takes the form "bb%d", deserialize with reference to actual object
				if type( argument ) == "string" and string.match( argument, "^t/bb%d$" ) then
					callback.arguments[ argumentIndex ] = bluebear.lot.get_object_by_cid( string.sub( argument, 3, string.len( argument ) ) )
				end
			end
		end
	end
end

bluebear.register_class( "system.object.base", Object )
