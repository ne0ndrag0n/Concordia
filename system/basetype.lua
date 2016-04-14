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

-- Private methods (do not override these!)
-- Called by the engine, runs all callbacks due for the given ticks
-- @param		{String}		currentTick
function _classes.object:_run( currentTick )
	-- Is there a _sys._sched entry for currentTick?
	local callbackList = self._sys._sched[ currentTick ]

	if type( callbackList ) == "table" then
		-- Clear the callback table from self._sys._sched
		self._sys._sched[ currentTick ] = nil

		-- Fire each callback in the table
		for index, callbackTable in ipairs( callbackList ) do
			self[ callbackTable.method ]( self, unpack( callbackTable.arguments ) )
		end
	end
end
