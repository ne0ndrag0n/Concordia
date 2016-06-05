--[[
  Base motives (game.motive.*) for the basic BlueBear game
--]]

local modpack_path = ...
local files = bluebear.util.get_directory_list( modpack_path )

for index, file in ipairs( files ) do
	if file.name ~= "obj.lua" then assert( loadfile( modpack_path.."/"..file.name ) )() end
end
