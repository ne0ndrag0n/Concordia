#ifndef ENGINE
#define ENGINE

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "bbtypes.hpp"
#include "lot.hpp"
#include <string>
#include <memory>
#include <vector>


namespace BlueBear {

	class Engine {

		private:
			static constexpr const char* LUASPHERE_MAIN = "system/main.lua";
			static constexpr const char* BLUEBEAR_MODPACK_DIRECTORY = "assets/modpacks/";
			static constexpr const unsigned int WORLD_TICKS_MAX = 500000;

			lua_State* L;
			unsigned int worldTicks;
			std::unique_ptr< Lot > currentLot;
			void createLotTable();
			void callActionOnObject( const char* playerId, const char* obejctId, const char* method );
			void deserializeFunctionRefs();

		public:
			Engine();
			~Engine();
			bool setupRootEnvironment();
			void objectLoop();
			bool loadLot( const char* lotPath );

	};

}

#endif
