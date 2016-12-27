--[[
	define promises: there may be several kinds of promises (e.g. timebased promises in pure Lua,
	or engine-based promises that require the C++ engine to signal for the callback)
--]]


bluebear.engine.require_modpack( "middleclass" )
bluebear.engine.require_modpack( "class" )

local Promise = class( 'system.promise.base' )

-- i'm willing to bet literally none of this shit will work on the first try

Promise.STATUS = { PENDING = 0, FULFILLED = 1, REJECTED = 2 }

function Promise:initialize( func )
	if type( func ) ~= "function" then
		error( "Must pass function to system.promise.base constructor" )
	end

	self.state = Promise.STATUS.PENDING

	-- value once promise is FULFILLED or REJECTED
	self.value = nil

	self.handlers = {}

	self:do_resolve( func )
end

-- If you encounter problems here, try making this a local function with args on_fulfilled and on_rejected,
-- and bind on_fulfilled/on_rejected using bluebear.util.bind( 'system.promise.base:resolve/reject', self )
function Promise:do_resolve( func )
	local done = false

	local success, error = pcall(
		func,
		-- TODO: Might want to deanonymize these functions into something that can be turned into an sfunction
		function( value )
			if done then return end
			done = true
			self:resolve( value )
		end,
		function( error )
			if done then return end
			done = true
			self:reject( error )
		end
	)

	if success == false then
		if done then return end
		done = true
		self:reject( error )
	end
end

local function get_then( value )
	if type( value ) == "table" and type( value.isInstanceOf ) == "function" and value:isInstanceOf( Promise ) then
		return value.then_call
	end

	return nil
end

function Promise:resolve( result )
	local success, error = pcall( function()
		local then_call = get_then( result )

		if then_call then
			self:do_resolve( bluebear.util.bind( then_call, result ) )
			return
		end

		self:fulfill( result )

	end )

	if success == false then
		self:reject( error )
	end

end

function Promise:fulfill( result )
	self.state = Promise.STATUS.FULFILLED
	self.value = result
	for index, handler in ipairs( self.handlers ) do
		self:handle( handler )
	end
	self.handlers = nil
end

function Promise:reject( error )
	self.state = Promise.STATUS.REJECTED
	self.value = error
	for index, handler in ipairs( self.handlers ) do
		self:handle( handler )
	end
	self.handlers = nil
end

function Promise:handle( handler )
	if self.state == Promise.STATUS.PENDING then
		table.insert( self.handlers, handler )
	else
		if self.state == Promise.STATUS.FULFILLED and type( handler.on_fulfilled ) == "function" then
			handler.on_fulfilled( self.value )
		end

		if self.state == Promise.STATUS.REJECTED and type( handler.on_rejected ) == "function" then
			handler.on_rejected( self.value )
		end
	end
end

function Promise:done( on_fulfilled, on_rejected )
	bluebear.engine.set_timeout(
		-- Schedule handle for the next tick
		bluebear.util.bind( 'system.promise.base:handle', self, { on_fulfilled = on_fulfilled, on_rejected = on_rejected } ), 1
	)
end

function Promise:then_call( on_fulfilled, on_rejected )
	local parent = self

	return Promise:new( function( resolve, reject )

			parent:done(
				function( result )
					if type( on_fulfilled ) == "function" then
						local success, f_result = pcall( resolve, on_fulfilled( result ) )

						if success then
							return f_result
						else
							return reject( f_result )
						end
					else
						return resolve( result )
					end
				end,
				function( error )
					if type( on_rejected ) == "function" then
						local success, f_result = pcall( resolve, on_rejected( error ) )

						if success then
							return f_result
						else
							return reject( f_result )
						end
					else
						return reject( error )
					end
				end
			)

	end )
end

bluebear.register_class( Promise )

-- Now register all the other types (Base Promise had to get done first)
local modpack_path = ...
local files = bluebear.util.get_directory_list( modpack_path )

for index, file in ipairs( files ) do
	if file.name ~= "obj.lua" then assert( loadfile( modpack_path.."/"..file.name ) )( Promise ) end
end
