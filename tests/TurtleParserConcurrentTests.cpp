#include <gtest/gtest.h>
#include <Dice/rdf_parser/Parser/Turtle/Parsers/ConcurrentStreamParser.hpp>

namespace {
    using namespace rdf_parser::Turtle::parsers;
    using namespace rdf_parser::store::rdf;
}


TEST(TurtleParserConcurrentTests,ntripleFileCon1) {
    CuncurrentStreamParser<> parser("../datasets/instances-labels.nt");
    auto it= parser.begin();
    while (it) {
        it++;
        //Do something
    }

}

TEST(TurtleParserConcurrentTests,turtleFileCon1) {
    CuncurrentStreamParser<> parser("../datasets/instance-types_transitive.ttl");
    auto it= parser.begin();
    while (it)
        it++;

}
