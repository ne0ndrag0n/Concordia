#ifndef ASYNCMGR
#define ASYNCMGR

#include <async++.h>
#include <memory>

namespace BlueBear {
  namespace Threading {
    class AsyncManager {
      public:
        std::unique_ptr< async::threadpool_scheduler > customThreadPool;
        bool useDefaultThreadPool;

        static AsyncManager& getInstance() {
          static AsyncManager instance;
          return instance;
        }

        async::threadpool_scheduler& getScheduler();

      private:
        AsyncManager();
        AsyncManager( AsyncManager const& );
        void operator=( AsyncManager const& );
    };
  }
}

#endif
