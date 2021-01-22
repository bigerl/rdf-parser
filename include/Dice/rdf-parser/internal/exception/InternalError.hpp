#ifndef RDF_PARSER_INTERNALERROR_HPP
#define RDF_PARSER_INTERNALERROR_HPP

namespace Dice::rdf_parser::internal::exception {
	class InternalError : public std::exception {
	public:
		[[nodiscard]] const char *what() const noexcept override {
			return "Internal error. This should not happen. Contact the maintainer";
		}
	};
}// namespace Dice::rdf_parser::internal::exception


#endif//RDF_PARSER_INTERNALERROR_HPP
