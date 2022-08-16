#pragma once

#include <cstdint>

namespace plog {

enum class FileWriterType : uint8_t { MMAPFILE = 0, APPENDFILE };

}
