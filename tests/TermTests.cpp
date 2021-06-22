#include <Dice/RDF/ParseTerm.hpp>
#include <Dice/rdf-parser/TurtleStringParser.hpp>
#include <gtest/gtest.h>

namespace Dice::tests::rdf_parser::term_tests {
	using namespace Dice::rdf_parser::internal::Turtle;
	using namespace Dice::rdf;
	using namespace Dice::rdf_parser::internal::Turtle::Parsers;
	using namespace Dice::rdf_parser;

	TEST(TermTests, compileHash) {
		std::unordered_set<Term> x;
		std::unordered_set<Triple> x2;
		std::unordered_set<sparql::Variable> y;
		std::unordered_set<sparql::TriplePattern> y2;
	}

	TEST(TermTests, compileFormattedOutput) {
		Term term = parse_term("<http://example.com/x>");
		std::cout << fmt::format("{}", term);
		Triple triple{parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>")};
		std::cout << fmt::format("{}", triple);

		sparql::TriplePattern triplePattern{parse_term("<http://example.com/x>"), sparql::Variable("my_var", false), sparql::Variable("my_var", true)};
		std::cout << fmt::format("{}", triplePattern);
	}

	TEST(TermTests, parseSingleIRI) {
		Term term = parse_term("<http://example.com/x>");
		ASSERT_EQ(term.type(), Term::NodeType::URIRef_);
	}

	TEST(TermTests, TripleEqual) {
		Triple triple1{parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>")};
		Triple triple2{parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>")};
		Triple triple3{parse_term("<http://example.com/not>"), parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>")};
		ASSERT_EQ(triple1, triple2);
		ASSERT_NE(triple1, triple3);
	}

	TEST(TermTests, parseTriple) {
		Triple triple{parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>"), parse_term("<http://example.com/x>")};
		for (const auto &term : triple) {
			ASSERT_EQ(term.type(), Term::NodeType::URIRef_);
		}
	}

	// TODO: Name of file's and tests' names seem odd.

	TEST(TermTests, parseIRI) {
		TurtleStringParser parser("@prefix pref: <http://example.com/> . "
								  "<http://example.com/x> a pref:x.");
		auto iterator = parser.begin();
		ASSERT_TRUE(bool(iterator));
		const Triple &triple = *iterator;
		// check if the non-prefix IRI is correct
		ASSERT_EQ(triple.subject().getIdentifier(), "<http://example.com/x>");
		// check if the prefix IRI is correct
		ASSERT_EQ(triple.subject().getIdentifier(), "<http://example.com/x>");
	}

	TEST(TermTests, parseStringTerm) {
		TurtleStringParser parser("@prefix xsd: <http://www.w3.org/2001/XMLSchema#> . "
								  "<http://example.com/x> a \"text\". "
								  "<http://example.com/x> a \"text\"^^<http://www.w3.org/2001/XMLSchema#string>. "
								  "<http://example.com/x> a \"text\"^^xsd:string. ");
		auto iterator = parser.begin();
		ASSERT_TRUE(bool(iterator));
		Triple plain = *iterator;
		++iterator;
		Triple full_typed = *iterator;
		++iterator;
		Triple prefix_typed = *iterator;

		// check if they are correct
		ASSERT_EQ(plain.object().getIdentifier(), "\"text\"");
		ASSERT_EQ(full_typed.object().getIdentifier(), "\"text\"");
		ASSERT_EQ(prefix_typed.object().getIdentifier(), "\"text\"");
	}


	TEST(TermTests, parseNumbers) {
		TurtleStringParser parser("@prefix : <http://example.org/elements> .                                                                              \n"
								  "<http://en.wikipedia.org/wiki/Helium>   "
								  ":atomicNumber 2 ;"
								  " :atomicMass 4.002602 ;"
								  " :specificGravity 1.663E-4 . ");
		auto iterator = parser.begin();
		ASSERT_TRUE(bool(iterator));
		Triple integerNumber = *iterator;
		++iterator;
		Triple decimalNumber = *iterator;
		++iterator;
		Triple doubleNumber = *iterator;

		// check if they are correct
		ASSERT_EQ(integerNumber.object().getIdentifier(), "\"2\"^^<http://www.w3.org/2001/XMLSchema#integer>");
		ASSERT_EQ(decimalNumber.object().getIdentifier(), "\"4.002602\"^^<http://www.w3.org/2001/XMLSchema#decimal>");
		ASSERT_EQ(doubleNumber.object().getIdentifier(), "\"1.663E-4\"^^<http://www.w3.org/2001/XMLSchema#double>");
	}
}// namespace Dice::tests::rdf_parser::term_tests
