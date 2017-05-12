#ifndef ENGINE
#define ENGINE

#include "scripting/event/waitingtable.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "bbtypes.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include <queue>
#include <jsoncpp/json/json.h>

namespace BlueBear {
	class EventManager;

	namespace Scripting {
		namespace LuaKit {
			class Serializer;
		}

		class Lot;
		class InfrastructureFactory;

		class Engine {

			private:
				static constexpr const char* LUASPHERE_MAIN = "system/root.lua";
				static constexpr const char* BLUEBEAR_MODPACK_DIRECTORY = "assets/modpacks/";
				static constexpr const char* SYSTEM_MODPACK_DIRECTORY = "system/modpacks/";
				static constexpr const char* MODPACK_MAIN_SCRIPT = "obj.lua";
				static constexpr const Tick WORLD_TICKS_MAX = 300;

				std::shared_ptr< EventManager > eventManager;

				std::chrono::time_point< std::chrono::steady_clock > lastExecuted;

				Tick currentTick;
				Tick ticksPerSecond;

				Event::WaitingTable waitingTable;

				std::unique_ptr< InfrastructureFactory > infrastructureFactory;
				const char* currentModpackDirectory;
				std::map< std::string, BlueBear::ModpackStatus > loadedModpacks;
				bool active;
				bool cancel;
				unsigned int sleepInterval;

				void callActionOnObject( const char* playerId, const char* objectId, const char* method );
				// TODO: New method to deserialise function refs will be needed in LuaKit::Serializer
				void processCommands();

				friend class LuaKit::Serializer;

			public:
				lua_State* L;

				std::vector< LuaReference > objects;
				std::shared_ptr< Lot > currentLot;

				Engine( std::shared_ptr< EventManager > eventManager );
				~Engine();
				void setupEvents();
				void objectLoop();
				bool loadLot( const char* lotPath );
				bool submitLuaContributions();
				void setActiveState( bool status );

				bool loadModpackSet( const char* modpackDirectory );
				bool loadModpack( const std::string& name );

				static int lua_loadModpack( lua_State* L );
				static int lua_setupStemcell( lua_State* L );
				static int lua_print( lua_State* L );
				static int lua_setTimeout( lua_State* L );
				static int lua_getLotObjects( lua_State* L );
				static int lua_getLotObjectsByType( lua_State* L );
		};
	}
}

#endif
