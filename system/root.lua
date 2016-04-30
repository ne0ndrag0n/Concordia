-- Provides the root environment for Lua scripts within the BlueBear engine
-- DO NOT MODIFY THIS FILE, or else everything will get goofy

bluebear = {

	-- Table containing methods to interface with the BlueBear lot (a primarily C++ construct)
	lot = nil,

	-- Table containing properties of the current engine (for now, just the current tick) usable by internal methods
	engine = {
		current_tick = 0
	}

};
