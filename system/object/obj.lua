--[[
	BlueBear base object
	This object is the object that all other Luasphere objects descend from - the base object
	Anything that can be placed on a lot and ran as an entity is an "object"
--]]

bluebear.engine.require_modpack( "middleclass" )
bluebear.engine.require_modpack( "class" )

local Object = class( 'system.object.base' )

function Object:load( saved )
	local data = saved or {}

	self._cid = data._cid

	if type( self._cid ) == 'string' then
		-- Every object needs to track cid on bluebear.util.lastcid
		-- when deserialising: the largest one we find is the new lastcid!
		local numericCid = tonumber( string.match( self._cid or "", "%d+" ) )
		if numericCid > bluebear.util.lastcid then
			bluebear.util.lastcid = numericCid
		end
	end
end

function Object:save()
	local out = {
		_cid = self._cid
	}

	return out
end

bluebear.register_class( Object )
