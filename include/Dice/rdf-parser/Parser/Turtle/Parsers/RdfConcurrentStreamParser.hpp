#ifndef RDF_PARSER_TURTLEPEGTLCONCURRENTSTREAMPARSER_HPP
#define RDF_PARSER_TURTLEPEGTLCONCURRENTSTREAMPARSER_HPP

/**
 * CuncurrentStreamParser is responsible for parsing Rdfs from stream sources.
 * It also creates its own thread for parsing.
 * It is also responsible for synchronizing between the parsing thread and the triples queue
 * It parse a file as a stream and put the parsed triples increasingly in a tbb::concurrent_bounded_queue
 * It is the best choice for very large files or stream sources.
 */

#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include <utility>

#include "Dice/rdf-parser/Parser/Turtle/Actions/Actions.hpp"
#include "Dice/rdf-parser/Parser/Turtle/Parsers/AbstractParser.hpp"
#include "Dice/rdf-parser/Parser/Turtle/States/ConcurrentState.hpp"
#include "Dice/rdf-parser/util/ScopedThread.hpp"

#include "Dice/rdf-parser/Parser/Turtle/Configurations.hpp"

namespace Dice::rdf_parser::Turtle::parsers {

    /*
     *
     */
    class RdfConcurrentStreamParser : public AbstractParser<RdfConcurrentStreamParser, false> {
        using Term = Dice::rdf::Term;
        using URIRef = Dice::rdf::URIRef;
        using Literal = Dice::rdf::Literal;
        using BNode = Dice::rdf::BNode;
        using Variable = Dice::sparql::Variable;
        using VarOrTerm = Dice::sparql::VarOrTerm;
        using Triple = Dice::rdf::Triple;
        using TriplePattern = Dice::sparql::TriplePattern;

    private:
        std::shared_ptr<
                boost::lockfree::spsc_queue<
                        Triple,
                        boost::lockfree::capacity<Configurations::RdfConcurrentStreamParser_QueueCapacity>>>
                parsedTerms;
        unsigned int upperThreshold;
        unsigned int lowerThreshold;

        std::ifstream stream;
        std::shared_ptr<std::condition_variable> cv;
        std::shared_ptr<std::mutex> m;
        std::shared_ptr<std::condition_variable> cv2;
        std::shared_ptr<std::mutex> m2;
        std::shared_ptr<std::atomic_bool> termCountWithinThresholds;
        std::shared_ptr<std::atomic_bool> termsCountIsNotEmpty;
        std::shared_ptr<std::atomic_bool> parsingIsDone;
        std::unique_ptr<util::ScopedThread> parsingThread;

    public:
        void startParsing(std::string filename, std::size_t bufferSize) {
            try {

                States::ConcurrentState<false>
                        state(parsedTerms,
                              upperThreshold,
                              cv, m,
                              cv2, m2,
                              termCountWithinThresholds,
                              termsCountIsNotEmpty,
                              parsingIsDone);
                tao::pegtl::parse<Grammar::grammar<false>, Actions::action>(
                        tao::pegtl::istream_input(stream, bufferSize, filename), std::move(state));
            } catch (std::exception &e) {
                throw std::exception(e);
            }
        }

        ~RdfConcurrentStreamParser() override {
            stream.close();
        }


        explicit RdfConcurrentStreamParser(const std::string &filename)
                : stream{filename},
                  upperThreshold(Configurations::RdfConcurrentStreamParser_QueueCapacity),
                  lowerThreshold(Configurations::RdfConcurrentStreamParser_QueueCapacity / 10),
                  parsedTerms{
                          std::make_shared<boost::lockfree::spsc_queue<Triple, boost::lockfree::capacity<Configurations::RdfConcurrentStreamParser_QueueCapacity>>>()},
                  cv{std::make_shared<std::condition_variable>()},
                  m{std::make_shared<std::mutex>()},
                  cv2{std::make_shared<std::condition_variable>()},
                  m2{std::make_shared<std::mutex>()},
                  termCountWithinThresholds{std::make_shared<std::atomic_bool>(false)},
                  termsCountIsNotEmpty{std::make_shared<std::atomic_bool>(false)},
                  parsingIsDone{std::make_shared<std::atomic_bool>(false)},
                  parsingThread{std::make_unique<util::ScopedThread>(
                          std::thread(&RdfConcurrentStreamParser::startParsing, this, filename,
                                      Configurations::RdfConcurrentStreamParser_BufferSize))} {}


        void nextTriple() override {
            parsedTerms->pop(*(this->current_triple));
            if (parsedTerms->read_available() < lowerThreshold) {
                {
                    std::lock_guard<std::mutex> lk(*m);
                    *termCountWithinThresholds = true;
                }
                cv->notify_one();
            }
        }

        bool hasNextTriple() const override {
            if (parsedTerms->read_available() != 0) {
                return true;
            } else {

                //check if the parsing is done
                if (*parsingIsDone) {
                    std::cout << "from main thread : parsingIsDone " << std::endl;
                    return false;
                } else {
                    std::unique_lock<std::mutex> lk(*m2);
                    *termsCountIsNotEmpty = false;
                    cv2->wait(lk, [&] { return termsCountIsNotEmpty->load(); });

                    if (parsedTerms->read_available() != 0)
                        return true;

                    else if (*parsingIsDone) {
                        std::cout << "from main thread : parsingIsDone " << std::endl;
                        return false;
                    } else {
                        throw std::runtime_error("");
                    }
                }
            };
        };


        Iterator<RdfConcurrentStreamParser, false> begin_implementation() {
            return Iterator<RdfConcurrentStreamParser, false>(this);
        }
    };
}// namespace Dice::rdf_parser::Turtle::parsers


#endif//RDF_PARSER_TURTLEPEGTLCONCURRENTSTREAMPARSER_HPP
