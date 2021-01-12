#include <gtest/gtest.h>

#include <Dice/rdf-parser/Parser/Turtle/Parsers/RdfConcurrentStreamParser.hpp>
#include <Dice/rdf-parser/Parser/Turtle/Parsers/RdfStreamParser.hpp>

namespace Dice::tests::rdf_parser::turtle_parser_concurrent_tests {
	using namespace rdf_parser::Turtle::parsers;
	using namespace rdf_parser::store::rdf;


	TEST(TurtleParserConcurrentTests, ntripleFileCon1) {
		RdfConcurrentStreamParser parser("../datasets/instances-labels.nt");
		auto it = parser.begin();
		while (it) {
			it++;
			//Do something
		}
	}

	TEST(TurtleParserConcurrentTests, turtleFileCon1) {
		RdfConcurrentStreamParser parser("../datasets/instance-types_transitive.ttl");
		auto it = parser.begin();
		while (it)
			it++;
	}
}// namespace Dice::tests::rdf_parser::turtle_parser_concurrent_tests