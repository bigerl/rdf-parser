#ifndef RDF_PARSER_ABSTRACTPARSER_HPP
#define RDF_PARSER_ABSTRACTPARSER_HPP

#include "Dice/RDF/Triple.hpp"
#include "Dice/SPARQL/TriplePattern.hpp"


/**
 * Base class for parsing triples from different sources.
 */

namespace Dice::rdf_parser::internal::Turtle::Parsers {

	template<class Parser, bool sparqlQuery>
	class Iterator;

	template<class Derived, bool sparqlQuery>
	class AbstractParser {
		using Term = Dice::rdf::Term;
		using VarOrTerm = Dice::sparql::VarOrTerm;
		using Triple = Dice::rdf::Triple;
		using TriplePattern = Dice::sparql::TriplePattern;
		using Triple_t = std::conditional_t<sparqlQuery, TriplePattern, Triple>;

	protected:
		using element_type = Triple_t;

		explicit AbstractParser() {
			current_triple ;
		};
		// TODO: doesn't need to be shared
		element_type current_triple;


	public:
		/**
         * process to the next parsed triple.
         */
		virtual void nextTriple() = 0;

		/**
         * check whether there is a further triple
         */
		[[nodiscard]] virtual bool hasNextTriple() const = 0;

		/**
         * get the current triple
         */
		const element_type &getCurrentTriple() {
			return current_triple;
		}


		virtual ~AbstractParser() = default;


		Iterator<Derived, sparqlQuery> begin() {
			return static_cast<Derived *>(this)->begin_implementation();
		}

		bool end() { return false; }
	};

	template<class Parser, bool sparqlQuery>
	class Iterator {
		using Term = Dice::rdf::Term;
		using Triple = Dice::rdf::Triple;
		using TriplePattern = Dice::sparql::TriplePattern;
		using Triple_t = std::conditional_t<sparqlQuery, TriplePattern, Triple>;

	private:
		bool done_;
		bool parser_done_;
		Parser *triplesParser = nullptr;

	public:
		explicit Iterator(Parser *triplesParser) : done_{false}, parser_done_{false}, triplesParser{triplesParser} {
			//check if there is at least one parsed triple
			if (triplesParser->hasNextTriple())
				this->operator++();
			else
				parser_done_ = true;
		};

		void operator++() {
			if (parser_done_) {
				done_ = true;
			} else {
				triplesParser->nextTriple();
				if (not triplesParser->hasNextTriple())
					parser_done_ = true;
			}
		}

		void operator++(int) { operator++(); }

		operator bool() { return not done_; }

		const Triple_t &operator*() { return triplesParser->getCurrentTriple(); }
	};


};// namespace Dice::rdf_parser::Turtle::parsers


#endif//RDF_PARSER_ABSTRACTPARSER_HPP
