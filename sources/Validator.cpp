#include "Validator.h"
#include <numeric>
#include <cwctype>
#include <regex>
#include "Error.h"

namespace az::cli
{

Validator& Validator::operator+=(const Validator& other)
{
	for (const auto& rule : other.rules) {
		rules[rule.first] = rule.second;
	}
	return *this;
}

Validator& Validator::unit(const std::string& unit)
{
	if (!unit.empty()) {
		rules[Rule::UNIT] = unit;
	}
	return *this;
}

Validator& Validator::boolean(const std::string& unit)
{
	rules[Rule::TYPE] = Value::Type::Bool;
	if (rules[Rule::GLOSSARY].empty()) {
		for (const auto& variant : Value::getBoolStrings()) {
			rules[Rule::GLOSSARY].append({variant, Value()});
		}
	}
	return this->unit(unit);
}

Validator& Validator::integer(const std::string& unit)
{
	rules[Rule::TYPE] = Value::Type::Integer;
	return this->unit(unit);
}

Validator& Validator::real(const std::string& unit)
{
	rules[Rule::TYPE] = Value::Type::Real;
	return this->unit(unit);
}

Validator& Validator::string(const std::string& unit)
{
	rules[Rule::TYPE] = Value::Type::String;
	return this->unit(unit);
}

Validator& Validator::nonempty()
{
	rules[Rule::NONEMPTY] = true;
	return *this;
}

Validator& Validator::one_of(const std::list<Value>& variants)
{
	rules[Rule::GLOSSARY].reset();
	for (const auto& variant : variants) {
		rules[Rule::GLOSSARY].append({variant, Value()});
	}
	return *this;
}

Validator& Validator::glossary(const std::list<std::pair<std::string,Value>>& glossary)
{
	rules[Rule::GLOSSARY].reset();
	for (const auto& term : glossary) {
		rules[Rule::GLOSSARY].append({term.first, term.second});
	}
	return *this;
}

Validator& Validator::pattern(const std::string& pattern)
{
	rules[Rule::PATTERN] = pattern;
	return *this;
}

Validator& Validator::no_duplicates_of(const std::string& symbols)
{
	rules[Rule::NO_DUPLICATES_OF] = symbols;
	return *this;
}

Validator& Validator::min(int64_t min)
{
	rules[Rule::MIN] = min;
	return *this;
}

Validator& Validator::max(int64_t max)
{
	rules[Rule::MAX] = max;
	return *this;
}

Validator& Validator::no_repeats_of(const std::string& chars)
{
	rules[Rule::NO_REPEATS_OF] = chars;
	return *this;
}

Validator& Validator::cannot_start_with(const std::string& chars)
{
	rules[Rule::CANNOT_START_WITH] = chars;
	return *this;
}

Validator& Validator::cannot_end_with(const std::string& chars)
{
	rules[Rule::CANNOT_END_WITH] = chars;
	return *this;
}

Validator& Validator::cannot_start_end_with(const std::string& chars)
{
	cannot_start_with(chars);
	cannot_end_with(chars);
	return *this;
}

Validator& Validator::trim(const std::string& chars)
{
	rules[Rule::TRIM] = chars;
	return *this;
}

Validator& Validator::prefix(const std::string& prefix)
{
	rules[Rule::PREFIX] = prefix;
	return *this;
}

Validator& Validator::suffix(const std::string& suffix)
{
	rules[Rule::SUFFIX] = suffix;
	return *this;
}

Validator& Validator::lower_case()
{
	rules[Rule::LOWER_CASE] = true;
	rules.erase(Rule::UPPER_CASE);
	return *this;
}

Validator& Validator::upper_case()
{
	rules[Rule::UPPER_CASE] = true;
	rules.erase(Rule::LOWER_CASE);
	return *this;
}

bool Validator::has(Rule rule) const
{
	return rules.count(rule) > 0;
}

Value Validator::get(Rule rule) const
{
	if (has(rule)) {
		return rules.at(rule);
	}
	return Value::Type::None;
}

void Validator::print(std::ostream& stream, const Value& value_by_default) const
{
	auto value_type = get(Rule::TYPE);
	auto value_unit = get(Rule::UNIT);

	bool has_unit = !value_unit.isNone() || !value_type.isNone();
	if (has_unit) {
		stream << " <";
		if (!value_unit.isNone()) {
			stream << value_unit;
		} else {
			stream << value_type.getTypeName();
		}
	}
	if (has(Rule::GLOSSARY)) {
		stream << " {";
		bool is_first = true;
		for (const auto& term : get(Rule::GLOSSARY)) {
			if (!is_first) {
				stream << ", ";
			}
			stream << term[0];
			if (term[0] == value_by_default) {
				stream << " (default)";
			}
			is_first = false;
		}
		stream << "}";
	}
	else if (has(Rule::MIN) || has(Rule::MAX)) {
		auto min = get(Rule::MIN);
		auto max = get(Rule::MAX);

		if (value_type.isNumber()) {
			stream << " {" << min << ".." << max << "}";
		}
	}
	if (!value_by_default.isNone() && !has(Rule::GLOSSARY)) {
		stream << " (default: " << value_by_default << ")";
	}
	if (has_unit) {
		stream << ">";
	}
}

std::string Validator::amend(const char* raw) const
{
	auto str = Value(raw).asWideString();
	if (str.empty()) {
		return raw;
	}
	for (const auto& suit : rules) {
		switch (suit.first) {
			case Rule::TRIM: {
				auto chars = suit.second.asWideString();
				auto cutter = [&chars](wchar_t ch) { return chars.find(ch) == chars.npos; };
				str.erase(str.begin(), std::find_if(str.begin(), str.end(), cutter));
			    str.erase(std::find_if(str.rbegin(), str.rend(), cutter).base(), str.end());
				break;
			}
			case Rule::PREFIX: {
				auto prefix = suit.second.asWideString();
				if (str.find(prefix) != 0) {
					str.insert(0, prefix);
				}
				break;
			}
			case Rule::SUFFIX: {
				auto suffix = suit.second.asWideString();
				if (str.rfind(suffix) != str.length() - suffix.length()) {
					str += suffix;
				}
				break;
			}
			case Rule::LOWER_CASE:
				std::for_each(str.begin(), str.end(), [](wchar_t& ch) { ch = std::towlower(ch); });
				break;
			case Rule::UPPER_CASE:
				std::for_each(str.begin(), str.end(), [](wchar_t& ch) { ch = std::towupper(ch); });
				break;
			default:
				break;
		}
	}
	return Value(str).asString();
}

bool Validator::check(const std::string& src) const
{
	if (src.empty() && get(Rule::NONEMPTY)) {
		throw Error(Error::Code::EmptyValue);
	}
	auto wsrc = Value(src).asWideString();
	auto type = get(Rule::TYPE);

	for (const auto& suit : rules) {
		switch (suit.first) {
			case Rule::MIN:
				if (type.isNumber()) {
					if (Value(src).convert(type) < suit.second) {
						throw Error(Error::Code::TooSmall).value(src).help(suit.second);
					}
				} else if (wsrc.length() < uint64_t(suit.second)) {
					throw Error(Error::Code::TooShort).value(src).help(suit.second);
				}
				break;
			case Rule::MAX:
				if (type.isNumber()) {
					if (Value(src).convert(type) > suit.second) {
						throw Error(Error::Code::TooLarge).value(src).help(suit.second);
					}
				} else if (wsrc.length() > uint64_t(suit.second)) {
					throw Error(Error::Code::TooLong).value(src).help(suit.second);
				}
				break;
			case Rule::PATTERN:
				try {
					if (suit.second.isString()) {
						std::wregex pattern(suit.second.asWideString());
						if (!std::regex_match(wsrc, pattern)) {
							throw Error(Error::Code::InvalidValue).value(src).help(suit.second);
						}
					}
				} catch (const std::regex_error&) {
					throw Error(Error::Code::InvalidRule).value(suit.second);
				}
				break;
			case Rule::NO_DUPLICATES_OF:
				for (const auto& symbol : suit.second.asWideString()) {
					if (std::count(wsrc.begin(), wsrc.end(), symbol) > 1) {
						throw Error(Error::Code::DuplicateChar).value(src).help(Value(symbol));
					}
				}
				break;
			case Rule::NO_REPEATS_OF: {
				auto suspects = suit.second.asWideString();
				for (auto prev = wsrc.begin(), curr = std::next(prev); curr != wsrc.end(); prev++, curr++) {
					if (*prev == *curr && suspects.find(*curr) != suspects.npos) {
						throw Error(Error::Code::RepetitiveChar).value(src).help(Value(*curr));
					}
				}
				break;
			}
			case Rule::CANNOT_START_WITH:
				if (suit.second.asWideString().find(wsrc.front()) != wsrc.npos) {
					throw Error(Error::Code::InvalidStart).value(src).help(Value(wsrc.front()));
				}
				break;
			case Rule::CANNOT_END_WITH:
				if (suit.second.asWideString().find(wsrc.back()) != wsrc.npos) {
					throw Error(Error::Code::InvalidEnd).value(src).help(Value(wsrc.back()));
				}
				break;
			case Rule::GLOSSARY:
				if (suit.second.isArray()) {
					const auto& glossary = suit.second;
					bool found = std::any_of(glossary.begin(), glossary.end(),
					    [&src](const Value& term) { return match(src, term[0]); });
					if (!found) {
						auto vars = std::accumulate(std::next(glossary.begin()), glossary.end(), glossary[0][0].asString(),
						    [](std::string sum, const Value& term) { return std::move(sum) + ", " + term[0].asString(); });
						throw Error(Error::Code::InvalidValue).value(src).help(vars);
					}
				}
				break;
			default:
				break;
		}
	}
	return true;
}

Value Validator::apply(const std::string& src) const
{
	Value value(src);
	for (const auto& rule : {Rule::GLOSSARY, Rule::TYPE}) {
		if (!has(rule)) {
			continue;
		}
		const auto& ruler = get(rule);
		switch (rule) {
			case Rule::GLOSSARY:
				if (ruler.isArray()) {
					for (const auto& term : ruler) {
						if (match(src, term[0])) {
							value = term[1].isNone() ? term[0] : term[1];
							break;
						}
					}
				}
				break;
			case Rule::TYPE:
				value.convert(ruler.getType());
				break;
			default:
				break;
		}
	}
	return value;
}

bool Validator::match(const std::string& string, const Value& value)
{
	try {
		return Value(string).convert(value) == value;
	}
	catch (const std::exception&) {
		return false;
	}
}

Validator evaluate()
{
	return Validator();
}

}
