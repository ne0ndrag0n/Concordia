#ifndef ENGINE
#define ENGINE

#include "bbtypes.hpp"
#include "lot.hpp"
#include <string>
#include <vector>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define BLUEBEAR_OBJECTS_DIRECTORY   "assets/objects/"
#define BLUEBEAR_TEMPLATES_DIRECTORY "assets/templates/"
#define BLUEBEAR_LOT_MAGIC_ID 		 0x42424C54
#define WORLD_TICKS_MAX				 604800000
#define	LUASPHERE_OBJECTS_TABLE		 "objects"
#define LUASPHERE_PLAYERS_TABLE		 "players"

namespace BlueBear {

	class Engine {
		
		private:
			lua_State* L;
			unsigned int worldTicks;
			Lot* currentLot;
			bool verifyODT( std::vector< BlueBear::OdtEntry > odt );
			int createLotTable( Lot* lot );
			std::string getTitleFromPopType( BlueBear::LotEntityType lotEntityType );
			void callActionOnObject( const char* playerId, const char* obejctId, const char* method );

		public:
			Engine();
			~Engine();
			bool setupRootEnvironment();
			void objectLoop();
			bool loadLot( const char* lotPath );
					
	};
	
}

#endif
