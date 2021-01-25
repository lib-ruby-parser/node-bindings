#ifndef LIB_RUBY_PARSER_NODE_RESULT_H
#define LIB_RUBY_PARSER_NODE_RESULT_H

#include <variant>
#include <string>

namespace lib_ruby_parser_node
{
#define UNWRAP_RESULT(var, call)       \
    auto result_##var = (call);        \
    if (result_##var.is_err())         \
    {                                  \
        return result_##var.get_err(); \
    }                                  \
    auto var = result_##var.get();

    template <typename T>
    class Result
    {
        std::variant<std::string, T> data;

    public:
        Result() = default;
        Result(T value) : data(std::move(value)){};
        Result(std::string error) : data(error){};
        Result(const char *error) : data(error){};

        bool is_ok()
        {
            return std::holds_alternative<T>(data);
        }

        bool is_err()
        {
            return std::holds_alternative<std::string>(data);
        }

        T
        get()
        {
            return std::move(std::get<T>(data));
        }

        std::string get_err()
        {
            return std::get<std::string>(data);
        }
    }; // namespace lib_ruby_parser_node
} // namespace lib_ruby_parser_node

#endif // LIB_RUBY_PARSER_NODE_RESULT_H
