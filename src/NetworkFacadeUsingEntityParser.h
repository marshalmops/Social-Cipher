#ifndef NETWORKFACADEUSINGENTITYPARSER_H
#define NETWORKFACADEUSINGENTITYPARSER_H

#include "EntityJsonParserInterface.h"

class NetworkFacadeUsingEntityParser
{
public:
    NetworkFacadeUsingEntityParser(const std::shared_ptr<EntityJsonParserInterface> &entityParser);
    
protected:
    std::shared_ptr<EntityJsonParserInterface> m_entityParser;
    
};

#endif // NETWORKFACADEUSINGENTITYPARSER_H
