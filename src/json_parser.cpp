#include "json_parser.h"

#include <stack>
#include <sstream>
#include <string>
#include <cctype>

namespace jfather
{
size_t OBJECT_INDEX = 0;
size_t ARRAY_INDEX = 1;
size_t STRING_INDEX = 2;
size_t INT_INDEX = 3;
size_t FLOAT_INDEX = 4;
size_t NULL_INDEX = 5;

std::string readString(size_t& read_index, const std::string& buf)
{
    size_t string_start = read_index;
    char c;
    while (true) // Can't end buf inside a string.
    {
        c = buf[read_index];
        if (c == '\\')
        {
            // escape character so read another.
            ++read_index;
        }
        else if (c == '"')
        {
            std::string s;
            size_t length = read_index - string_start;
            ++read_index;
            return buf.substr(string_start, length);
        }
        ++read_index;
    }
    //PRINT_ERROR("JSON Parsing: Reached end of file parsing string");
    throw 0;
}

 template<typename ValueT>
 inline void HandleValue(std::stack<JSONObject*>& nodeStack, ValueT&& v)
{
     auto& top_data = nodeStack.top()->data;
     if (top_data.index() == NULL_INDEX)
     {
         top_data = std::move(v);
         nodeStack.pop();
     }
     else // if  (top_data.index() == ARRAY_INDEX)
     {
         top_data.emplace<ValueT>(std::move(v));
     }
}

JSONObject ParseJson(const std::string& str)
{
    size_t read_index = 0;
    JSONObject root{};

    std::stack<JSONObject*> nodeStack;
    
    // Parse root object/array
    nodeStack.push(&root);
    char c = str[read_index]; 
    ++read_index;
    if (c == '{')
    {
        root.data.emplace<JSONObject::object_type>();
    }
    else if (c == '[')
    {
        root.data.emplace<JSONObject::array_type>();
    }

    while (read_index < str.size())
    {
        c = str[read_index];
        ++read_index;

        // for debugging.
        //std::cout << c;

        switch (c)
        {
        case ' ':
            //Bleed off whitespace more efficiently. Alas this optimization does nothing noticeable.
            //while (*reinterpret_cast<const long long *>(str.data() + read_index) == 0x2020202020202020)
            //{
            //    read_index += 8;
            //}
            //while (*reinterpret_cast<const long*>(str.data() + read_index) == 0x20202020)
            //{
            //    read_index += 4;
            //}
            while (str[read_index] == ' ')
            {
                ++read_index;
            }
            break;
        case '{':
        {
            auto& top_data = nodeStack.top()->data;
            if (top_data.index() == NULL_INDEX)
            {
                // Assume top() repreents our item, and we need to now set it as an object.
                top_data.emplace<JSONObject::object_type>();
            }
            else if (top_data.index() == ARRAY_INDEX)
            {
                // This is an object in an array. Construct it, set it as an object, and push it.
                JSONObject::array_type& array = std::get<JSONObject::array_type>(top_data);
                JSONObject& element = array.emplace_back();
                element.data.emplace<JSONObject::object_type>();
                nodeStack.push(&element);
            }
            break;
        }
        case '"':
        {
            auto& top_data = nodeStack.top()->data;
            if (top_data.index() == OBJECT_INDEX)
            {
                // This string is a key. Insert a new pair into the containing object,
                // default constructing the value.
                JSONObject::object_type& obj = std::get<JSONObject::object_type>(top_data);
                auto pair = obj.emplace(readString(read_index, str), JSONObject());
                pair.first->second.data.emplace<NullValue>();
                // Push the value into the stack as the next thing to parse.
                // We leave it typeless.
                nodeStack.push(&(pair.first->second));
            }
            else if (top_data.index() == NULL_INDEX)
            {
                // We are reading a string value type. We would already have placed our JSONObject
                // on the stack, so now we just set it.
                top_data = readString(read_index, str);
                // Our string is now resolved.
                nodeStack.pop();
            }
            else if (top_data.index() == ARRAY_INDEX)
            {
                // Push string into array and leave stack as is.
                JSONObject::array_type& array = std::get<JSONObject::array_type>(top_data);
                JSONObject& item = array.emplace_back();
                item.data = readString(read_index, str);
            }
            break;
        }
        case '[':
        {
            auto& top_data = nodeStack.top()->data;
            if (top_data.index() == NULL_INDEX)
            {
                // This is a value in an array. It would have already been pushed.
                top_data.emplace<JSONObject::array_type>();
            }
            else if (top_data.index() == ARRAY_INDEX)
            {
                // This is an array in an array. Construct it, set it as an object, and push it.
                JSONObject::array_type& array = std::get<JSONObject::array_type>(top_data);
                JSONObject& element = array.emplace_back();
                element.data.emplace<JSONObject::array_type>();
                nodeStack.push(&element);
            }
            break;
        }
        case '}':
        case ']':
        {
            nodeStack.pop();
            break;
        }
        default:
            if (c == 'E' || c == 'e' || c == '-' || c == '+' || c == '.' || std::isdigit(c))
            {
                // Handling a number
                std::stringstream ss;
                bool is_float = false;
                // TODO: Make fast.
                while (c == 'E' || c == 'e' || c == '-' || c == '+' || c == '.' || std::isdigit(c))
                {
                    if (c == '.')
                    {
                        is_float = true;
                    }
                    ss << c;
                    c = str[read_index];
                    ++read_index;
                }
                if (is_float)
                {
                    double d;
                    ss >> d;
                    HandleValue(nodeStack, std::move(d));
                }
                else
                {
                    long long i;
                    ss >> i;
                    HandleValue(nodeStack, std::move(i));
                }
            }
            else if (c == 'n')
            {
                read_index+=3; // rest of "null"
                auto& top_data = nodeStack.top()->data;
                HandleValue(nodeStack, nullptr);

            }
            else if (c == 't')
            {
                read_index+=3; // rest of "true"
                HandleValue(nodeStack, true);
            }
            else if (c == 'f')
            {
                read_index+=4; // rest of "false"
                HandleValue(nodeStack, false);
            }
            break;
        }
    }
    return root;
}

}