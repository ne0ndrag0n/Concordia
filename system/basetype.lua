-- Base Types

_classes.object = newclass();

function _classes.object:load( saved )
	local data = saved or {}

	self._cid = data._cid
	self._sys = data._sys
end

function _classes.object:save()
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

function _classes.object:setup()
	-- abstract - This is the "real" constructor of objects
end

--[[
	Sleep numTicks amount of ticks and then call the function on this object named by functionName
--]]
function _classes.object:sleep( numTicks )
	local ticks = numTicks + bluebear.engine.current_tick

	-- do nothing but return a new promise with the ticks set to "ticks"
	return _classes.promise.base( self, ticks )
end

--[[
	Enter in a callback on this object's _sys._sched table for the destination tick
--]]
function _classes.object:register_callback( tick, method, wrapped_arguments )
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
function _classes.object:_run()
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
function _classes.object:_deserialize_function_refs()
	print( self._cid.." is gonna deserialize" )
end

-- be careful: if the methods are NOT virtual, "self" will not be the same self when you call it!
-- all methods should be made virtual by default by some type of modification to the yaci lib
_classes.object:virtual( "sleep" )
_classes.object:virtual( "_run" )

-- define promises: there may be several kinds of promises (e.g. timebased promises in pure Lua,
-- or engine-based promises that require the C++ engine to signal for the callback)
_classes.promise = {
	base = newclass()
}

function _classes.promise.base:init( obj_ref, start_tick )
	self.object = obj_ref
	self.next_tick = start_tick
end

function _classes.promise.base:then_call( func_name, ... )
	self.object:register_callback( self.next_tick, func_name, { ... } )

	-- any future "then_call" statements will be called tick per tick
	self.next_tick = self.next_tick + 1

	return self
end
