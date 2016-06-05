local Promise = ...
-- TODO: PathfinderPromise calls methods on bluebear.engine to find a path and call back
local PathfinderPromise = bluebear.extend( 'system.promise.base', 'system.promise.pathfinder' )

function PathfinderPromise:initialize( origin, target )
	-- Pathfinder finds path from object (origin) to destination (target)
	Promise.initialize( self, origin )

	self.destination = target

	-- then_call will put these onto the promise, when the engine gets done finding the path it will
	-- run through this table and call 'em all, each waiting one tick between them all.
	self.thens = {}

	-- TODO: the actual thing. right now this just does a simple defer. consider this for Picasso milestone.
	-- the below snippet doesn't go in this method, it goes somewhere else
	--[[
	bluebear.engine.find_path( self.object, self.destination, function( result )
 		-- do the shit where we unload "result" as a series of object navigations across the map
		-- now call all your thens
		for index, callback in ipairs( self.thens ) do

		end
	end )
	--]]
end

bluebear.register_class( PathfinderPromise )
