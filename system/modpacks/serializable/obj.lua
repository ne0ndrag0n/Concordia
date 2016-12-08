--[[
	BlueBear base object
	This object is the object that all other Luasphere objects descend from - the base object
	Anything that can be placed on a lot and ran as an entity is an "object"
--]]

bluebear.engine.require_modpack( "util" )
bluebear.engine.require_modpack( "middleclass" )
bluebear.engine.require_modpack( "class" )

local Object = class( 'system.serializable.base' )

function Object:initialize( serialized )

	if type( serialized ) == "table" then
		-- De-serialising an item (call the load method)
		self:load( serialized )
	else
		-- Creating a brand-new item
		self._cid = bluebear.util.get_cid()
	end

	-- TODO: What goes here? Register the item globally using bluebear.engine.__private.reg_inst( self ). This method will take
	-- the current table and add it to the std::map tracking all serializable instances floating around in the Luaverse
end

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
