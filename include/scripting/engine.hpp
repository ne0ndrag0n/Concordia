#ifndef ENGINE
#define ENGINE

#include "scripting/serializableinstance.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "bbtypes.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <list>

namespace BlueBear {
	namespace Threading {
		class CommandBus;
	}

	namespace Scripting {
		class Lot;
		class EventManager;
		class InfrastructureFactory;

		class Engine {

			private:
				static constexpr const char* LUASPHERE_MAIN = "system/root.lua";
				static constexpr const char* BLUEBEAR_MODPACK_DIRECTORY = "assets/modpacks/";
				static constexpr const char* SYSTEM_MODPACK_DIRECTORY = "system/modpacks/";
				static constexpr const char* MODPACK_MAIN_SCRIPT = "obj.lua";
				static constexpr const Tick WORLD_TICKS_MAX = 300;

				lua_State* L;
				Tick currentTick;
				Tick ticksPerSecond;
				std::shared_ptr< Lot > currentLot;
				std::unique_ptr< EventManager > eventManager;
				std::unique_ptr< InfrastructureFactory > infrastructureFactory;
				const char* currentModpackDirectory;
				std::map< std::string, BlueBear::ModpackStatus > loadedModpacks;
				Threading::CommandBus& commandBus;
				bool active;
				bool cancel;
				unsigned int sleepInterval;

				void callActionOnObject( const char* playerId, const char* objectId, const char* method );
				bool deserializeFunctionRefs();
				void setupLotEnvironment();
				void processCommands();

			public:
				std::map< std::string, std::unique_ptr< SerializableInstance > > objects;

				Engine( Threading::CommandBus& commandBus );
				~Engine();
				void objectLoop();
				bool loadLot( const char* lotPath );
				bool setupRootEnvironment();
				void setActiveState( bool status );

				bool loadModpackSet( const char* modpackDirectory );
				bool loadModpack( const std::string& name );
				int getLotObjectByCid( const std::string& cid );
				int createSerializableInstanceFromJSON( const Json::Value& serialEntity );
				int createSerializableInstance( const std::string& classID );

				static int lua_loadModpack( lua_State* L );
				static int lua_setupStemcell( lua_State* L );
				static int lua_print( lua_State* L );

				static int lua_createSerializableInstance( lua_State* L );
				static int lua_getLotObjectByCid( lua_State* L );
				static int lua_getLotObjects( lua_State* L );
				static int lua_getLotObjectsByType( lua_State* L );

				class Command {
					public:
						virtual void execute( Engine& instance ) = 0;
				};

				class RegisterInstance : public Command {
					unsigned int instanceId;

					public:
						RegisterInstance( unsigned int instanceId );
						void execute( Engine& instance );
				};

				class SetLockState : public Command {
					bool status;
					public:
						SetLockState( bool status );
						void execute( Engine& instance );
				};

				class ShutdownEngine : public Command {
					public:
						void execute( Engine& instance );
				};

				using CommandList = std::list< std::unique_ptr< Command > >;
		};
	}
}

#endif
