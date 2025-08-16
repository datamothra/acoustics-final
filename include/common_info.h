#ifndef COMMON_INFO_H
#define COMMON_INFO_H

#include "bn_span.h"
#include "bn_string_view.h"

namespace common
{
    struct info
    {
        bn::span<const bn::string_view> text_lines;
        const char* title;
        const char* subtitle;
    };
}

#endif
