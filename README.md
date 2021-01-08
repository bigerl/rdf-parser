# Tentris RDF Parser

This is the RDF parser used by [Tentris](https://github.com/dice-group/tentris). It uses [PEGTL](https://github.com/taocpp/PEGTL), Parsing Expression Grammar Library, to parse RDF files (currently supported: ntriple, turtle).
 It also supports parsing the triplesBlock part of Sparql queries.

It has buffered streaming support so that you can load files that are too big to fit in main memory. 

Currently it is not fully supporting comments.it fails to parse comments in some scenarios .

## how to build
### prerequisites

install conan, cmake and a C++17 compiler

add conan remotes
```
conan remote add conan "https://api.bintray.com/conan/conan/public-conan" ; conan remote add taocpp https://api.bintray.com/conan/taocpp/public-conan ; conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
 
```

### build
```
mkdir build
cd build
conan install .. --build=missing --settings compiler.libcxx="libstdc++11"
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make -j
```


### conan 

recipe : rdf-parser/0.11@dice-group/stable
```
conan remote add dice "https://api.bintray.com/conan/dice-group/tentris"

```

## Documentation

### Introduction

A header-only library designed to allow efficient and controlled parsing for RDF languages.
Currently it supports ntriples and turtle languages, as well as triplesBlock part of sparql queries.
It uses tao's PEGTL library for parsing.

###  Usage

The library is easy to use, for parsing turtle or ntriples include the header for the required parser.
for example, to use the RdfConcurrentStreamParser, include `<Dice/rdf_parser/Parser/Turtle/Parsers/RdfConcurrentStreamParser.hpp>` 

Rdf parsers' constructor accept one string parameter which is either the string which is the document to be parsed in case of `StringParser` or the filename of the file that contains the document to be parsed on the other cases

Example :
```c++
 RdfConcurrentStreamParser parser("datasets/g.nt");
```
where `"datasets/g.nt"` is the file name of the file we want to parse.

Now an iterator can be created:
```c++
auto iterator=parser.begin();
```

We can iterate over the triples and used the triples:

```c++
while(it)
{
 Triple triple= *it;
 //do something
 it++;
}
```

### Parsers types
There are four types of parsers which can be used:
- `RdfStringParser`: It is used for parsing Rdf Strings immediately. It accepts one parameter which is the string of the document to be parsed.
- `RdfFileParser`: It is used for parsing a whole document file that contains a Rdf on it  at a time. The parsed triples cannot be accessed before the parsing of the whole document is done. It is faster than the `RdfStreamParser` (below) but require much more memory and thus may run out of memory for large inputs.
- `RdfStreamParser`: It is used for parsing streams or large files that contains a Rdf. It can process very big files with low memory usage by parsing chunk by chunk. However, triples are not accessible until the parsing is done. 
- `RdfConcurrentStreamParser`: It can be used the same way as the `RdfStreamParser` but uses a separated thread for parsing and uses Intel's TBB `concurrent_queue`
for storing the parsed triples. Therefore, the already parsed triples can be accessed during the parsing process. It is the recommended parser to use for large files or streams and for simultaneous parsing and accessing to the already parsed triples.

- `TriplesBlockStringParser`: It is used for parsing Sparql's TripleBlocks Strings immediately. It accepts one parameter which is the string of the document to be parsed.
- `TriplesBlockFileParser`: It is used for parsing a whole document file that contains Sparql's TripleBlocks at a time. The parsed elements cannot be accessed before the parsing of the whole document is done.

### Examples

1-Here we create a full example for parsing a turtle file. we use here the `RdfConcurrentStreamParser`. We assuming there is an turtle file at `datasets/dataset1.ttl`. The file is parsed and the triples are printed to `std::out`.

```c++
#include <Dice/rdf_parser/Parser/Turtle/Parsers/RdfConcurrentStreamParser.hpp>

namespace {
    using namespace rdf_parser::Turtle::parsers;
    using namespace rdf_parser::store::rdf;
}

int main()
{
 RdfConcurrentStreamParser parser("datasets/dataset1.ttl");
 auto it=Parser.begin();
 while (it){
         Triple triple= *it;
         std::cout << triple.subject().getIdentifier() << " "
                   << triple.predicate().getIdentifier() << " "
                   << triple.object().getIdentifier()
                   << std::endl;
         it++;
     }
}
```

2-Here we create a full example for parsing The tripleBlock part of SparqlQuery Using the TriplesBlockStringParser.

```c++
#include <Dice/rdf_parser/Parser/Turtle/Parsers/TriplesBlockStringParser.hpp>
 
namespace {
    using namespace rdf_parser::Turtle::parsers;
    using namespace  rdf_parser::SparqlQuery;
}
int main()
{
//create a map of prefixes
std::map<std::string,std::string> prefixes;
prefixes.insert(std::pair<std::string,std::string>("wde","http://www.wikidata.org/entity/"));
prefixes.insert(std::pair<std::string,std::string>("wdt","http://www.wikidata.org/prop/direct/"));

// create the parser with 2 parameters: the query and the prefixes map
TriplesBlockStringParser parser("?var1 <http://www.wikidata.org/prop/P463> _:b0 . _:b0 <http://www.wikidata.org/prop/statement/P463> wde:Q202479 ; <http://www.wikidata.org/prop/qualifier/P580> ?var2 .",prefixes) ;

//get an iterator 
auto it= parser.begin();

while (it)
{
    TriplePatternElement triplePatternElement=*it;
    VarOrTerm subject=TriplePatternElement.subject();
    VarOrTerm subject=TriplePatternElement.predicate();
    VarOrTerm subject=TriplePatternElement.object();
    it++;
}
}
```
