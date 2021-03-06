#ifndef DICE_RDF_TERM_HPP
#define DICE_RDF_TERM_HPP


#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#include <Dice/hash/DiceHash.hpp>
#include <fmt/format.h>


namespace Dice::rdf {

	/**
     * This is a portable a string view. It is not bound to a single string but applicable to any copy of a string.
     */
	struct unbound_string_view {
		std::ptrdiff_t start = 0;
		std::size_t count = 0;

		/**
         * Get a string_view for the given string.
         * @param str string to be viewed
         * @return string_view on the string
         */
		[[nodiscard]] std::string_view string_view(const std::string &str) const {
			return {str.c_str() + start, count};
		}
	};

	class Literal;

	class BNode;

	class URIRef;

	/**
     * An RDF Term.
     */
	class Term {
	public:
		/**
         * Type of a Term.
         */
		enum NodeType {
			None = 0,
			URIRef_,
			BNode_,
			Literal_

		};

	protected:
		std::string identifier_{};
		NodeType node_type_{};
		unbound_string_view value_{};
		// TODO: we may use std::variant for _lang and _data_type
		unbound_string_view lang_{};
		unbound_string_view data_type_{};

		/**
         * Is to being used by subtypes URIRef, BNode and Literal.
         * @param identifier
         * @param node_type
         */
		Term(std::string identifier, NodeType node_type) : identifier_(std::move(identifier)),
														   node_type_(node_type) {}

	public:
		Term() = default;

		Term(Term &) = default;

		Term(const Term &) = default;

		Term(Term &&) = default;

		Term &operator=(const Term &) = default;

		Term &operator=(Term &&) = default;


		[[nodiscard]] const std::string &getIdentifier() const {
			return identifier_;
		}

		[[nodiscard]] inline const NodeType &type() const {
			return node_type_;
		}

		[[nodiscard]] inline bool isLiteral() const {
			return node_type_ == NodeType::Literal_;
		}

		[[nodiscard]] inline bool isBNode() const {
			return node_type_ == NodeType::BNode_;
		}

		[[nodiscard]] inline bool isURIRef() const {
			return node_type_ == NodeType::URIRef_;
		}

		[[nodiscard]] inline Literal &castLiteral();

		[[nodiscard]] inline BNode &castBNode();

		[[nodiscard]] inline URIRef &castURIRef();

		[[nodiscard]] inline const Literal &castLiteral() const;

		[[nodiscard]] inline const BNode &castBNode() const;

		[[nodiscard]] inline const URIRef &castURIRef() const;

		[[nodiscard]] inline std::string_view value() const {
			return value_.string_view(identifier_);
		}

		inline bool operator==(const Term &rhs) const {
			return identifier_ == rhs.identifier_;
		}

		inline bool operator!=(const Term &rhs) const {
			return identifier_ != rhs.identifier_;
		}

		inline bool operator<(const Term &rhs) const {
			return identifier_ < rhs.identifier_;
		}

		inline bool operator>(const Term &rhs) const {
			return identifier_ > rhs.identifier_;
		}

		friend bool operator==(const Term &lhs, const std::unique_ptr<Term> &rhs) {
			return lhs == *rhs;
		}

		friend bool operator==(const std::unique_ptr<Term> &lhs, const Term &rhs) {
			return *lhs == rhs;
		}

		friend bool operator==(const std::unique_ptr<Term> &lhs, const std::unique_ptr<Term> &rhs) {
			return *lhs == *rhs;
		}

		friend bool operator==(const Term *lhs, const std::unique_ptr<Term> &rhs) {
			return *lhs == *rhs;
		}

		friend bool operator==(const std::unique_ptr<Term> &lhs, const Term *rhs) {
			return *lhs == *rhs;
		}

		[[nodiscard]] std::size_t hash() const {
			return ::Dice::hash::dice_hash(this->identifier_);
		}
	};

	class URIRef : public Term {

	public:
		explicit URIRef(const std::string &uri) : Term(fmt::format("<{}>", uri), NodeType::URIRef_) {
			this->value_ = {1, uri.size()};
		};

		[[nodiscard]] inline std::string_view uri() const {
			return value();
		}
	};

	class BNode : public Term {
	public:
		explicit BNode(const std::string &bnode_label) : Term(fmt::format("_:{}", bnode_label), NodeType::BNode_) {
			this->value_ = {2, bnode_label.size()};
		};

		[[nodiscard]] inline std::string_view bnodeLabel() const {
			return value();
		}
	};

	class Literal : public Term {
	public:
		Literal(const std::string &value, const std::optional<std::string> &lang,
				const std::optional<std::string> &type) {
			node_type_ = NodeType::Literal_;
			if (lang) {
				this->identifier_ = fmt::format("\"{}\"@{}", value, lang.value());
				this->lang_ = {(std::ptrdiff_t)(1 + value.size() + 1 + 1), lang->size()};
			} else if (type and (type != "http://www.w3.org/2001/XMLSchema#string")) {
				// TODO: handle default cases
				this->identifier_ = fmt::format("\"{}\"^^<{}>", value, type.value());
				this->data_type_ = {(std::ptrdiff_t)(1 + value.size() + 1 + 2 + 1), type->size()};
				// TODO: manage types types
			} else {
				this->identifier_ = fmt::format("\"{}\"", value);
			}
			this->value_ = {1, value.size()};
		}

		[[nodiscard]] inline std::string_view dataType() const {
			return data_type_.string_view(identifier_);
		}

		[[nodiscard]] inline std::string_view lang() const {
			return lang_.string_view(identifier_);
		}

		[[nodiscard]] inline bool hasDataType() const {
			return data_type_.count != 0;
		}

		[[nodiscard]] inline bool hasLang() const {
			return lang_.count != 0;
		}
	};

	Literal &Term::castLiteral() {
		return static_cast<Literal &>(*this);
	}

	BNode &Term::castBNode() {
		return static_cast<BNode &>(*this);
	}

	URIRef &Term::castURIRef() {
		return static_cast<URIRef &>(*this);
	}

	const Literal &Term::castLiteral() const {
		return static_cast<const Literal &>(*this);
	}

	const BNode &Term::castBNode() const {
		return static_cast<const BNode &>(*this);
	}

	const URIRef &Term::castURIRef() const {
		return static_cast<const URIRef &>(*this);
	}
};// namespace Dice::rdf

namespace Dice::hash {
	template<>
	inline std::size_t dice_hash(const Dice::rdf::Term &v) noexcept {
		return v.hash();
	}
}// namespace Dice::hash

template<>
struct std::hash<Dice::rdf::Term> {
	size_t operator()(const Dice::rdf::Term &v) const {
		return v.hash();
	}
};

template<>
struct fmt::formatter<const Dice::rdf::Term *> {
	template<typename ParseContext>
	constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

	template<typename FormatContext>
	auto format(const Dice::rdf::Term *p, FormatContext &ctx) {
		if (p != nullptr)
			return format_to(ctx.out(), p->getIdentifier());
		else
			return format_to(ctx.out(), "");
	}
};

template<>
struct fmt::formatter<Dice::rdf::Term> {
	template<typename ParseContext>
	constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

	template<typename FormatContext>
	auto format(const Dice::rdf::Term &p, FormatContext &ctx) {
		return format_to(ctx.out(), p.getIdentifier());
	}
};

#endif//DICE_RDF_TERM_HPP