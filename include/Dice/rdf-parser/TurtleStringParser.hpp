
#ifndef RDF_PARSER_TURTLEPEGTLSTRINGPARSER_HPP
#define RDF_PARSER_TURTLEPEGTLSTRINGPARSER_HPP


/**
 * StringParser is responsible for parsing rdf from string sources.
 */


#include <chrono>
#include <utility>

#include "Dice/rdf-parser/internal/Turtle/Actions/Actions.hpp"
#include "Dice/rdf-parser/internal/Turtle/Parsers/BaseStringParser.hpp"


namespace Dice::rdf_parser {

	class TurtleStringParser : public internal::Turtle::Parsers::BaseStringParser<false> {


	public:
		using Iterator [[maybe_unused]] = internal::Turtle::Parsers::Iterator<TurtleStringParser, false>;

		/**
         * The constructor start the parsing.if the input is not valid it will throws and exception.
         * it also invoke nextTriple to have the first triple ready for using .
         * @param text the string to parse
         */
		explicit TurtleStringParser(std::string text) : BaseStringParser<false>(std::move(text)) {}

		/**
         * checks whether a string is valid rdf turtle file
         */
		static bool isParsable(const std::string &input) {
			namespace Grammar = internal::Turtle::Grammar;
			try {
				tao::pegtl::string_input in(input, "the text");
				tao::pegtl::parse<Grammar::grammar<false>>(in);
				return true;
			} catch (std::exception &e) {
				return false;
			}
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


		~TurtleStringParser() override = default;
	};
}// namespace Dice::rdf_parser

#endif//RDF_PARSER_TURTLEPEGTLSTRINGPARSER_HPP
