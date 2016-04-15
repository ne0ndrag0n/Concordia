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

	return out
end

function _classes.object:setup()
	-- abstract - This is the "real" constructor of objects
end

--[[
	Sleep numTicks amount of ticks and then call the function on this object named by functionName
--]]
function _classes.object:sleep( method, numTicks, ... )
	local ticksKey = tostring( numTicks + bluebear.engine.current_tick )
	local ticksTable = self._sys._sched[ ticksKey ]
	local arguments = { ... }

	-- If the ticksTable does not exist, create it
	if ticksTable == nil then
		self._sys._sched[ ticksKey ] = {}
		ticksTable = self._sys._sched[ ticksKey ]
	end

	-- Add this to the list of callbacks due for that tick
	table.insert( ticksTable, { method = method, arguments = arguments } )
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
-- be careful: if the methods are NOT virtual, "self" will not be the same self when you call it!
-- all methods should be made virtual by default by some type of modification to the yaci lib
_classes.object:virtual( "sleep" )
_classes.object:virtual( "_run" )
