#include "AJSP.hpp"
#include "PathConstructor.hpp"
#include <functional>
#include <map>
#include "VectorCollector.hpp"

class MapCollector: private AJSP::Listener
{
    public:
		using Predicate = std::function<bool(const std::string& path)>;

        MapCollector(Predicate p = True<std::string>);
        ~MapCollector();

        void reset();
        AJSP::Parser::Result parse(char c) {return _parser.parse(c);}
        bool done() const {return _parser.done();}

        const std::map<std::string, std::string>& getValues() const {return _values;}

    private:
        virtual void value(const std::string& value, AJSP::Parser::Entity entity);

        AJSP::Parser                		 _parser;
        std::map<std::string, std::string>   _values;
        Predicate							 _predicate;
};
