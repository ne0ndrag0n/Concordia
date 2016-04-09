--[[
--testing some shit
local test = {
	prop = "spam",
	table = {
		spam = "real spam",
		eggs = "real eggs",
		nested = {
			nest = nil
		}
	},
	method = function( self )
		self.table.nested.nest = "defined!"
	end
};

local Kitchen = _bblib.extend( newclass(), test );
local kitchen = Kitchen();
kitchen.table = _bblib.deep_copy( kitchen.table );
kitchen:method();
--]]
