#ifndef ENGINE
#define ENGINE

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "bbtypes.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>


namespace BlueBear {
	class Lot;
	class EventManager;

	class Engine {

		private:
			static constexpr const char* LUASPHERE_MAIN = "system/root.lua";
			static constexpr const char* BLUEBEAR_MODPACK_DIRECTORY = "assets/modpacks/";
			static constexpr const char* SYSTEM_MODPACK_DIRECTORY = "system/";
			static constexpr const char* MODPACK_MAIN_SCRIPT = "obj.lua";
			static constexpr const Tick WORLD_TICKS_MAX = 300;

			lua_State* L;
			Tick currentTick;
			Tick ticksPerSecond;
			std::shared_ptr< Lot > currentLot;
			std::unique_ptr< EventManager > eventManager;
			const char* currentModpackDirectory;
			std::map< std::string, BlueBear::ModpackStatus > loadedModpacks;

			void callActionOnObject( const char* playerId, const char* objectId, const char* method );
			bool deserializeFunctionRefs();
			void setupLotEnvironment();

		public:
			Engine();
			~Engine();
			void objectLoop();
			bool loadLot( const char* lotPath );
			bool setupRootEnvironment();

			bool loadModpackSet( const char* modpackDirectory );
			bool loadModpack( const std::string& name );
			static int lua_loadModpack( lua_State* L );
			static int lua_setupStemcell( lua_State* L );
	};

}

#endif
