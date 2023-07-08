// Basic representation of a JSON Object natively.

#ifndef JFATHER_JSON_H
#define JFATHER_JSON_H

#include <map>
#include <list>
#include <variant>
#include <string>

namespace jfather
{

// Used to specify an actual null literal (and not the string "null").
// Not to be confused with nullptr_t which is used for a JSONObject that has not otherwise been set to a type.
struct NullValue {};

struct JSONObject
{
    using object_type = std::map<std::string, struct JSONObject>;
    using array_type = std::list<struct JSONObject>;
    using data_type = std::variant<object_type, array_type, std::string, long long, double, NullValue, nullptr_t, bool>;

    data_type data;
};

} // namespace jfather
#endif // JFATHER_JSON_H
