#ifndef CMEBASEWIDGET
#define CMEBASEWIDGET

#include <map>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class CMEWidget {
      public:
        std::map< std::string, std::string > staticProperties;
      };

    }
  }
}

#endif
