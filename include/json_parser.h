// Fast? Json parser

#ifndef JFATHER_JSON_PARSER_H
#define JFATHER_JSON_PARSER_H

#include "json.h"

#include <string>

namespace jfather
{

// Parse the passed string into a json object. 
// Does *absolutely no validation*, and largely doesn't care about
// unneeded punctuation like ':' and ','.
JSONObject ParseJson(const std::string& str);

} // namespace jfather
#endif // JFATHER_JSON_PARSER_H
