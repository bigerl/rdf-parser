//
// Created by fakhr on 08.01.21.
//

#ifndef RDF_PARSER_TRIPLESBLOCKFILEPARSER_HPP
#define RDF_PARSER_TRIPLESBLOCKFILEPARSER_HPP


#include "BaseFileParser.hpp"

/**
 * TriplesBlockFileParser is responsible for parsing sparql's tripleBlocks from file sources.
 */

namespace {
    using namespace tao::pegtl;
}


namespace rdf_parser::Turtle::parsers {

    class TriplesBlockFileParser : public BaseFileParser<true> {


    public:


        /**
         * The constructor start the parsing.if the input is not valid it will throws and exception.
         * it also invoke nextTriple to have the first triple ready for using .
         * @param filename the filename of the file we want to parse
         */
        TriplesBlockFileParser(std::string text) : BaseFileParser<true>(text) {}


        /**
        * The constructor start the parsing.if the input is not valid it will throws and exception.
        * it also invoke nextTriple to have the first triple ready for using .
        * @param text the string to parse
        * @param prefix_map defines prefixes to be added before parsing
        */
        TriplesBlockFileParser(std::string text, std::map<std::string, std::string> prefix_map) : BaseFileParser<true>(text,
                                                                                                                 prefix_map) {};
    };
}
#endif //RDF_PARSER_TRIPLESBLOCKFILEPARSER_HPP
