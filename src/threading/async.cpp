#include "threading/async.hpp"
#include "configmanager.hpp"
#include <async++.h>
#include <memory>

namespace BlueBear {
  namespace Threading {

    AsyncManager::AsyncManager() {
      // If threadpool_detect is set (default), this will use the environment variable LIBASYNC_NUM_THREADS, or the number
      // of CPUs present on the system. The latter is preferred and the game will scale as the years progress.
      useDefaultThreadPool = ConfigManager::getInstance().getBoolValue( "threadpool_detect" );

      if( !useDefaultThreadPool ) {
        // Create a customThreadPool using setting threadpool_count (2 by default). This is not normally used unless you supertune
        // BlueBear using settings.json options "threadpool_detect: false" and "threadpool_count: x", where x is the custom
        // number of threads you wish to use.
        customThreadPool = std::make_unique< async::threadpool_scheduler >( ConfigManager::getInstance().getIntValue( "threadpool_count" ) );
      }
    }

    async::threadpool_scheduler& AsyncManager::getScheduler() {
      return useDefaultThreadPool ? async::default_threadpool_scheduler() : *customThreadPool;
    }

  }
}
