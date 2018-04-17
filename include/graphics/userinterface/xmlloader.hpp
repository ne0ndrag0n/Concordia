#ifndef GUI_XML_LOADER
#define GUI_XML_LOADER

#include "exceptions/genexc.hpp"
#include <tinyxml2.h>
#include <vector>
#include <memory>
#include <string>

namespace BlueBear::Graphics::UserInterface {
  class Element;

  class XMLLoader {
    tinyxml2::XMLDocument document;

    std::shared_ptr< Element > getElementFromXML( const tinyxml2::XMLElement* element );

  public:
    EXCEPTION_TYPE( FailedToLoadXMLException, "Failed to parse XML!" );
    EXCEPTION_TYPE( UnknownElementException, "Unknown UI element encountered!" );

    XMLLoader( const std::string& subject, bool file = true );

    std::vector< std::shared_ptr< Element > > getElements();
  };

}

#endif
