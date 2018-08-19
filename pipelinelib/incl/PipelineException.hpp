#pragma once

#include <exception>
#include <ostream>

namespace mfep {
namespace Pipeline {

class PipelineException : public std::runtime_error {
public:
    PipelineException(const char* message, const char* function, const char* file, int line) noexcept;
    void print       (std::ostream& stream) const;

private:
    const char* const m_message;
    const char* const m_function;
    const char* const m_file;
    const int         m_line;
};
#define PIPELINE_EXCEPTION(message) PipelineException(message,__PRETTY_FUNCTION__,__FILE__,__LINE__)

}   // namespace Pipeline
}   // namespace mfep
