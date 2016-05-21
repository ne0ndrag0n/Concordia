#ifndef CONFIGMANAGER
#define CONFIGMANAGER

#include <map>

namespace BlueBear {
  class ConfigManager {
    public:
      static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
      }
    private:
      ConfigManager() {}
      ConfigManager( ConfigManager const& );
      void operator=( ConfigManager const& );
  };
}


#endif
