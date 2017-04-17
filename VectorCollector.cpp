#include "VectorCollector.hpp"

using namespace AJSP;

VectorCollector::VectorCollector()
{  
    _parser.setListener(this);
}

VectorCollector::~VectorCollector()
{
}

void VectorCollector::reset()
{
    _values.clear();
    _parser.reset();
}
     
void VectorCollector::value(const std::string& value, Parser::Entity entity)
{
    _values.emplace_back(_parser.getCurrentPath(), value, entity);
}

    