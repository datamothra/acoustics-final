#ifndef COMMON_VARIABLE_8x16_SPRITE_FONT_H
#define COMMON_VARIABLE_8x16_SPRITE_FONT_H

#include "bn_sprite_font.h"
#include "bn_utf8_characters_map.h"
#include "bn_sprite_items_common_variable_8x16_font.h"

namespace common
{
    constexpr bn::utf8_characters_map variable_8x16_sprite_font_utf8_characters_map(
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:!?()[]+-*/<>=.,;'\" ^v"
    );

    constexpr bn::sprite_font variable_8x16_sprite_font(
        bn::sprite_items::common_variable_8x16_font,
        variable_8x16_sprite_font_utf8_characters_map
    );
}

#endif
