#pragma once

namespace man
{

typedef Array<uint8_t> String;

template <>
typedef Shared<Array<uint8_t>> Shared<String>;

}
