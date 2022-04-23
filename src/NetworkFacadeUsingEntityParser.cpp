#include "NetworkFacadeUsingEntityParser.h"

NetworkFacadeUsingEntityParser::NetworkFacadeUsingEntityParser(const std::shared_ptr<EntityJsonParserInterface> &entityParser)
    : m_entityParser{entityParser}
{
    
}
