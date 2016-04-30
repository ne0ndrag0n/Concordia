-- Specify the core bluebear object, of which all system modpacks will assume exists

bluebear = {

	-- Table containing methods to interface with the BlueBear lot (a primarily C++ construct)
	-- This is created at runtime by C++
	lot = nil,

	-- Table containing properties of the current engine
	engine = {
		current_tick = 0
	}

}
