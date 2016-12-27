local Promise = ...
local TimedPromise = bluebear.extend( 'system.promise.base', 'system.promise.timer' )

--[[
  A TimedPromise is simply a promise that resolves after the given time interval. This Promise will either go unfulfilled, or resolve, but never fail.
--]]

function TimedPromise:initialize( timeout )
  return Promise.initialize( self, function( resolve )
    bluebear.engine.set_timeout( bluebear.util.bind( 'system.promise.base:resolve', self ), timeout )
  end )
end

bluebear.register_class( TimedPromise )
