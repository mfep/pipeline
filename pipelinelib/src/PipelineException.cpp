#include "PipelineException.hpp"

using namespace mfep::Pipeline;

PipelineException::PipelineException(const char* message, const char* function,
                                                     const char* file, int line) noexcept :
    m_message(message), m_function(function), m_file(file), m_line(line)
{
}

void PipelineException::print(std::ostream& stream) const {
    stream << "Exception thrown at function \"" << m_function << "\" in file " << m_file << ":" << m_line << '\n'
           << "Message: " << m_message << '\n';
}
