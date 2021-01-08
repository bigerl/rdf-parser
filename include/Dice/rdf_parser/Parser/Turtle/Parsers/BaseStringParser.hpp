//
// Created by fakhr on 08.01.21.
//

#ifndef RDF_PARSER_BASESTRINGPARSER_HPP
#define RDF_PARSER_BASESTRINGPARSER_HPP



#include <chrono>

#include "TriplesParser.hpp"
#include "Dice/rdf_parser/Parser/Turtle/Actions/Actions.hpp"


/**
 * BaseStringParser is a base class for parsing strings .
 *
 */

namespace {
    using namespace tao::pegtl;
}


namespace rdf_parser::Turtle::parsers {

    template<bool sparqlQuery>
    class BaseStringParser : public TriplesParser<BaseStringParser<sparqlQuery>,sparqlQuery> {


    protected:
        /**
         * a queue for storing parsed triples .
         */
        std::shared_ptr<std::queue<std::conditional_t<sparqlQuery,SparqlQuery::TriplePatternElement ,Triple>>> parsedTerms;

    protected:

        /**
         * The constructor start the parsing.if the input is not valid it will throws and exception.
         * it also invoke nextTriple to have the first triple ready for using .
         * @param text the string to parse
         */
        BaseStringParser(std::string text) {
            try {
                string_input input(text, "the text");
                States::State<sparqlQuery> state(parsedTerms);
                parse<Grammer::grammer<sparqlQuery>, Actions::action>(input, state);

            }
            catch (std::exception &e) {
                throw e;
            }


        }

        /**
        * The constructor start the parsing.if the input is not valid it will throws and exception.
        * it also invoke nextTriple to have the first triple ready for using .
        * @param text the string to parse
        * @param prefix_map defines prefixes to be added before parsing
        */
        BaseStringParser(std::string text, std::map<std::string, std::string> prefix_map) {
            try {
                string_input input(text, "the text");
                States::State<sparqlQuery> state(parsedTerms);
                for (auto pair : prefix_map)
                    state.addPrefix(pair.first, pair.second);
                parse<Grammer::grammer<sparqlQuery>, Actions::action>(input, state);

            }
            catch (std::exception &e) {
                throw e;
            }

        }

    public:

        bool hasNextTriple() const override {
            return not parsedTerms->empty();
        }

        ~BaseStringParser() override {

        }

        void nextTriple() override {
            *(this->current_triple) = parsedTerms->front();
            parsedTerms->pop();

        }

        /**
         * checks whether a string is valid rdf turtle file
         */
        static bool isParsable(const std::string &input) {
            try {
                string_input in(input, "the text");
                parse<Grammer::grammer<sparqlQuery>>(in);
                return true;
            }
            catch (std::exception &e) {
                return false;
            }

        }

        Iterator<BaseStringParser,sparqlQuery> begin_implementation(){
            return Iterator<BaseStringParser,sparqlQuery>(this);
        }


        /**
         * calculate the time for parsing a rdf turtle string.
         * Note that the calculated time is only for parsing without using processing the input(creating and storing the triples out of the string)
         */
        static long calculateParsingTime(const std::string &input) {
            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            isParsable(input);
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            return duration;
        }

    };
}


#endif //RDF_PARSER_BASESTRINGPARSER_HPP