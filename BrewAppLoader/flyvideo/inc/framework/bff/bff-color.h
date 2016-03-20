/*
 * Copyright (c) 2008 Takahiko Kawasaki
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * @english
 * Color names.
 * @endenglish
 *
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_COLOR_H
#define BFF_COLOR_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEDisp.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
#define BFF_RGB_ALICE_BLUE               MAKE_RGB(0xF0,0xF8,0xFF)
#define BFF_RGB_ALICEBLUE                BFF_RGB_ALICE_BLUE
#define BFF_RGB_ANTIQUE_WHITE            MAKE_RGB(0xFA,0xEB,0xD7)
#define BFF_RGB_ANTIQUEWHITE             BFF_RGB_ANTIQUE_WHITE
#define BFF_RGB_AQUA                     MAKE_RGB(0x00,0xFF,0xFF)
#define BFF_RGB_AQUAMARINE               MAKE_RGB(0x7F,0xFF,0xD4)
#define BFF_RGB_AZURE                    MAKE_RGB(0xF0,0xFF,0xFF)
#define BFF_RGB_BEIGE                    MAKE_RGB(0xF5,0xF5,0xDC)
#define BFF_RGB_BISQUE                   MAKE_RGB(0xFF,0xE4,0xC4)
#define BFF_RGB_BLACK                    MAKE_RGB(0x00,0x00,0x00)
#define BFF_RGB_BLANCHED_ALMOND          MAKE_RGB(0xFF,0xEB,0xCD)
#define BFF_RGB_BLANCHEDALMOND           BFF_RGB_BLANCHED_ALMOND
#define BFF_RGB_BLUE                     MAKE_RGB(0x00,0x00,0xFF)
#define BFF_RGB_BLUE_VIOLET              MAKE_RGB(0x8A,0x2B,0xE2)
#define BFF_RGB_BLUEVIOLET               BFF_RGB_BLUE_VIOLET
#define BFF_RGB_BROWN                    MAKE_RGB(0xA5,0x2A,0x2A)
#define BFF_RGB_BURLY_WOOD               MAKE_RGB(0xDE,0xB8,0x87)
#define BFF_RGB_BURLYWOOD                BFF_RGB_BURLY_WOOD
#define BFF_RGB_CADET_BLUE               MAKE_RGB(0x5F,0x9E,0xA0)
#define BFF_RGB_CADETBLUE                BFF_RGB_CADET_BLUE
#define BFF_RGB_CHARTREUSE               MAKE_RGB(0x7F,0xFF,0x00)
#define BFF_RGB_CHOCOLATE                MAKE_RGB(0xD2,0x69,0x1E)
#define BFF_RGB_CORAL                    MAKE_RGB(0xFF,0x7F,0x50)
#define BFF_RGB_CORNFLOWER_BLUE          MAKE_RGB(0x64,0x95,0xED)
#define BFF_RGB_CORNFLOWERBLUE           BFF_RGB_CORNFLOWER_BLUE
#define BFF_RGB_CORNSILK                 MAKE_RGB(0xFF,0xF8,0xDC)
#define BFF_RGB_CRIMSON                  MAKE_RGB(0xDC,0x14,0x3C)
#define BFF_RGB_CYAN                     MAKE_RGB(0x00,0xFF,0xFF)
#define BFF_RGB_DARK_BLUE                MAKE_RGB(0x00,0x00,0x8B)
#define BFF_RGB_DARKBLUE                 BFF_RGB_DARK_BLUE
#define BFF_RGB_DARK_CYAN                MAKE_RGB(0x00,0x8B,0x8B)
#define BFF_RGB_DARKCYAN                 BFF_RGB_DARK_CYAN
#define BFF_RGB_DARK_GOLDEN_ROD          MAKE_RGB(0xB8,0x86,0x0B)
#define BFF_RGB_DARKGOLDENROD            BFF_RGB_DARK_GOLDEN_ROD
#define BFF_RGB_DARK_GRAY                MAKE_RGB(0xA9,0xA9,0xA9)
#define BFF_RGB_DARKGRAY                 BFF_RGB_DARK_GRAY
#define BFF_RGB_DARK_GREY                BFF_RGB_DARK_GRAY
#define BFF_RGB_DARKGREY                 BFF_RGB_DARK_GREY
#define BFF_RGB_DARK_GREEN               MAKE_RGB(0x00,0x64,0x00)
#define BFF_RGB_DARKGREEN                BFF_RGB_DARK_GREEN
#define BFF_RGB_DARK_KHAKI               MAKE_RGB(0xBD,0xB7,0x6B)
#define BFF_RGB_DARKKHAKI                BFF_RGB_DARK_KHAKI
#define BFF_RGB_DARK_MAGENTA             MAKE_RGB(0x8B,0x00,0x8B)
#define BFF_RGB_DARKMAGENTA              BFF_RGB_DARK_MAGENTA
#define BFF_RGB_DARK_OLIVE_GREEN         MAKE_RGB(0x55,0x6B,0x2F)
#define BFF_RGB_DARKOLIVEGREEN           BFF_RGB_DARK_OLIVE_GREEN
#define BFF_RGB_DARKORANGE               MAKE_RGB(0xFF,0x8C,0x00)
#define BFF_RGB_DARK_ORCHID              MAKE_RGB(0x99,0x32,0xCC)
#define BFF_RGB_DARKORCHID               BFF_RGB_DARK_ORCHID
#define BFF_RGB_DARK_RED                 MAKE_RGB(0x8B,0x00,0x00)
#define BFF_RGB_DARKRED                  BFF_RGB_DARK_RED
#define BFF_RGB_DARK_SALMON              MAKE_RGB(0xE9,0x96,0x7A)
#define BFF_RGB_DARKSALMON               BFF_RGB_DARK_SALMON
#define BFF_RGB_DARK_SEA_GREEN           MAKE_RGB(0x8F,0xBC,0x8F)
#define BFF_RGB_DARKSEAGREEN             BFF_RGB_DARK_SEA_GREEN
#define BFF_RGB_DARK_SLATE_BLUE          MAKE_RGB(0x48,0x3D,0x8B)
#define BFF_RGB_DARKSLATEBLUE            BFF_RGB_DARK_SLATE_BLUE
#define BFF_RGB_DARK_SLATE_GRAY          MAKE_RGB(0x2F,0x4F,0x4F)
#define BFF_RGB_DARKSLATEGRAY            BFF_RGB_DARK_SLATE_GRAY
#define BFF_RGB_DARK_SLATE_GREY          BFF_RGB_DARK_SLATE_GRAY
#define BFF_RGB_DARKSLATEGREY            BFF_RGB_DARK_SLATE_GREY
#define BFF_RGB_DARK_TURQUOISE           MAKE_RGB(0x00,0xCE,0xD1)
#define BFF_RGB_DARKTURQUOISE            BFF_RGB_DARK_TURQUOISE
#define BFF_RGB_DARK_VIOLET              MAKE_RGB(0x94,0x00,0xD3)
#define BFF_RGB_DARKVIOLET               BFF_RGB_DARK_VIOLET
#define BFF_RGB_DEEP_PINK                MAKE_RGB(0xFF,0x14,0x93)
#define BFF_RGB_DEEPPINK                 BFF_RGB_DEEP_PINK
#define BFF_RGB_DEEP_SKY_BLUE            MAKE_RGB(0x00,0xBF,0xFF)
#define BFF_RGB_DEEPSKYBLUE              BFF_RGB_DEEP_SKY_BLUE
#define BFF_RGB_DIM_GRAY                 MAKE_RGB(0x69,0x69,0x69)
#define BFF_RGB_DIMGRAY                  BFF_RGB_DIM_GRAY
#define BFF_RGB_DIM_GREY                 BFF_RGB_DIM_GRAY
#define BFF_RGB_DIMGREY                  BFF_RGB_DIM_GREY
#define BFF_RGB_DODGER_BLUE              MAKE_RGB(0x1E,0x90,0xFF)
#define BFF_RGB_DODGERBLUE               BFF_RGB_DODGER_BLUE
#define BFF_RGB_FIRE_BRICK               MAKE_RGB(0xB2,0x22,0x22)
#define BFF_RGB_FIREBRICK                BFF_RGB_FIRE_BRICK
#define BFF_RGB_FLORAL_WHITE             MAKE_RGB(0xFF,0xFA,0xF0)
#define BFF_RGB_FLORALWHITE              BFF_RGB_FLORAL_WHITE
#define BFF_RGB_FOREST_GREEN             MAKE_RGB(0x22,0x8B,0x22)
#define BFF_RGB_FORESTGREEN              BFF_RGB_FOREST_GREEN
#define BFF_RGB_FUCHSIA                  MAKE_RGB(0xFF,0x00,0xFF)
#define BFF_RGB_GAINSBORO                MAKE_RGB(0xDC,0xDC,0xDC)
#define BFF_RGB_GHOST_WHITE              MAKE_RGB(0xF8,0xF8,0xFF)
#define BFF_RGB_GHOSTWHITE               BFF_RGB_GHOST_WHITE
#define BFF_RGB_GOLD                     MAKE_RGB(0xFF,0xD7,0x00)
#define BFF_RGB_GOLDEN_ROD               MAKE_RGB(0xDA,0xA5,0x20)
#define BFF_RGB_GOLDENROD                BFF_RGB_GOLDEN_ROD
#define BFF_RGB_GRAY                     MAKE_RGB(0x80,0x80,0x80)
#define BFF_RGB_GREY                     BFF_RGB_GRAY
#define BFF_RGB_GREEN                    MAKE_RGB(0x00,0x80,0x00)
#define BFF_RGB_GREEN_YELLOW             MAKE_RGB(0xAD,0xFF,0x2F)
#define BFF_RGB_GREENYELLOW              BFF_RGB_GREEN_YELLOW
#define BFF_RGB_HONEY_DEW                MAKE_RGB(0xF0,0xFF,0xF0)
#define BFF_RGB_HONEYDEW                 BFF_RGB_HONEY_DEW
#define BFF_RGB_HOT_PINK                 MAKE_RGB(0xFF,0x69,0xB4)
#define BFF_RGB_HOTPINK                  BFF_RGB_HOT_PINK
#define BFF_RGB_INDIAN_RED               MAKE_RGB(0xCD,0x5C,0x5C)
#define BFF_RGB_INDIANRED                BFF_RGB_INDIAN_RED
#define BFF_RGB_INDIGO                   MAKE_RGB(0x4B,0x00,0x82)
#define BFF_RGB_IVORY                    MAKE_RGB(0xFF,0xFF,0xF0)
#define BFF_RGB_KHAKI                    MAKE_RGB(0xF0,0xE6,0x8C)
#define BFF_RGB_LAVENDER                 MAKE_RGB(0xE6,0xE6,0xFA)
#define BFF_RGB_LAVENDER_BLUSH           MAKE_RGB(0xFF,0xF0,0xF5)
#define BFF_RGB_LAVENDERBLUSH            BFF_RGB_LAVENDER_BLUSH
#define BFF_RGB_LAWN_GREEN               MAKE_RGB(0x7C,0xFC,0x00)
#define BFF_RGB_LAWNGREEN                BFF_RGB_LAWN_GREEN
#define BFF_RGB_LEMON_CHIFFON            MAKE_RGB(0xFF,0xFA,0xCD)
#define BFF_RGB_LEMONCHIFFON             BFF_RGB_LEMON_CHIFFON
#define BFF_RGB_LIGHT_BLUE               MAKE_RGB(0xAD,0xD8,0xE6)
#define BFF_RGB_LIGHTBLUE                BFF_RGB_LIGHT_BLUE
#define BFF_RGB_LIGHT_CORAL              MAKE_RGB(0xF0,0x80,0x80)
#define BFF_RGB_LIGHTCORAL               BFF_RGB_LIGHT_CORAL
#define BFF_RGB_LIGHT_CYAN               MAKE_RGB(0xE0,0xFF,0xFF)
#define BFF_RGB_LIGHTCYAN                BFF_RGB_LIGHT_CYAN
#define BFF_RGB_LIGHT_GOLDEN_ROD_YELLOW  MAKE_RGB(0xFA,0xFA,0xD2)
#define BFF_RGB_LIGHTGOLDENRODYELLOW     BFF_RGB_LIGHT_GOLDEN_ROD_YELLOW
#define BFF_RGB_LIGHT_GRAY               MAKE_RGB(0xD3,0xD3,0xD3)
#define BFF_RGB_LIGHTGRAY                BFF_RGB_LIGHT_GRAY
#define BFF_RGB_LIGHT_GREY               BFF_RGB_LIGHT_GRAY
#define BFF_RGB_LIGHTGREY                BFF_RGB_LIGHT_GREY
#define BFF_RGB_LIGHT_GREEN              MAKE_RGB(0x90,0xEE,0x90)
#define BFF_RGB_LIGHTGREEN               BFF_RGB_LIGHT_GREEN
#define BFF_RGB_LIGHT_PINK               MAKE_RGB(0xFF,0xB6,0xC1)
#define BFF_RGB_LIGHTPINK                BFF_RGB_LIGHT_PINK
#define BFF_RGB_LIGHT_SALMON             MAKE_RGB(0xFF,0xA0,0x7A)
#define BFF_RGB_LIGHTSALMON              BFF_RGB_LIGHT_SALMON
#define BFF_RGB_LIGHT_SEA_GREEN          MAKE_RGB(0x20,0xB2,0xAA)
#define BFF_RGB_LIGHTSEAGREEN            BFF_RGB_LIGHT_SEA_GREEN
#define BFF_RGB_LIGHT_SKY_BLUE           MAKE_RGB(0x87,0xCE,0xFA)
#define BFF_RGB_LIGHTSKYBLUE             BFF_RGB_LIGHT_SKY_BLUE
#define BFF_RGB_LIGHT_SLATE_GRAY         MAKE_RGB(0x77,0x88,0x99)
#define BFF_RGB_LIGHTSLATEGRAY           BFF_RGB_LIGHT_SLATE_GRAY
#define BFF_RGB_LIGHT_SLATE_GREY         BFF_RGB_LIGHT_SLATE_GRAY
#define BFF_RGB_LIGHTSLATEGREY           BFF_RGB_LIGHT_SLATE_GREY
#define BFF_RGB_LIGHT_STEEL_BLUE         MAKE_RGB(0xB0,0xC4,0xDE)
#define BFF_RGB_LIGHTSTEELBLUE           BFF_RGB_LIGHT_STEEL_BLUE
#define BFF_RGB_LIGHT_YELLOW             MAKE_RGB(0xFF,0xFF,0xE0)
#define BFF_RGB_LIGHTYELLOW              BFF_RGB_LIGHT_YELLOW
#define BFF_RGB_LIME                     MAKE_RGB(0x00,0xFF,0x00)
#define BFF_RGB_LIME_GREEN               MAKE_RGB(0x32,0xCD,0x32)
#define BFF_RGB_LIMEGREEN                BFF_RGB_LIME_GREEN
#define BFF_RGB_LINEN                    MAKE_RGB(0xFA,0xF0,0xE6)
#define BFF_RGB_MAGENTA                  MAKE_RGB(0xFF,0x00,0xFF)
#define BFF_RGB_MAROON                   MAKE_RGB(0x80,0x00,0x00)
#define BFF_RGB_MEDIUM_AQUA_MARINE       MAKE_RGB(0x66,0xCD,0xAA)
#define BFF_RGB_MEDIUMAQUAMARINE         BFF_RGB_MEDIUM_AQUA_MARINE
#define BFF_RGB_MEDIUM_BLUE              MAKE_RGB(0x00,0x00,0xCD)
#define BFF_RGB_MEDIUMBLUE               BFF_RGB_MEDIUM_BLUE
#define BFF_RGB_MEDIUM_ORCHID            MAKE_RGB(0xBA,0x55,0xD3)
#define BFF_RGB_MEDIUMORCHID             BFF_RGB_MEDIUM_ORCHID
#define BFF_RGB_MEDIUM_PURPLE            MAKE_RGB(0x93,0x70,0xD8)
#define BFF_RGB_MEDIUMPURPLE             BFF_RGB_MEDIUM_PURPLE
#define BFF_RGB_MEDIUM_SEA_GREEN         MAKE_RGB(0x3C,0xB3,0x71)
#define BFF_RGB_MEDIUMSEAGREEN           BFF_RGB_MEDIUM_SEA_GREEN
#define BFF_RGB_MEDIUM_SLATE_BLUE        MAKE_RGB(0x7B,0x68,0xEE)
#define BFF_RGB_MEDIUMSLATEBLUE          BFF_RGB_MEDIUM_SLATE_BLUE
#define BFF_RGB_MEDIUM_SPRING_GREEN      MAKE_RGB(0x00,0xFA,0x9A)
#define BFF_RGB_MEDIUMSPRINGGREEN        BFF_RGB_MEDIUM_SPRING_GREEN
#define BFF_RGB_MEDIUM_TURQUOISE         MAKE_RGB(0x48,0xD1,0xCC)
#define BFF_RGB_MEDIUMTURQUOISE          BFF_RGB_MEDIUM_TURQUOISE
#define BFF_RGB_MEDIUM_VIOLET_RED        MAKE_RGB(0xC7,0x15,0x85)
#define BFF_RGB_MEDIUMVIOLETRED          BFF_RGB_MEDIUM_VIOLET_RED
#define BFF_RGB_MIDNIGHT_BLUE            MAKE_RGB(0x19,0x19,0x70)
#define BFF_RGB_MIDNIGHTBLUE             BFF_RGB_MIDNIGHT_BLUE
#define BFF_RGB_MINT_CREAM               MAKE_RGB(0xF5,0xFF,0xFA)
#define BFF_RGB_MINTCREAM                BFF_RGB_MINT_CREAM
#define BFF_RGB_MISTY_ROSE               MAKE_RGB(0xFF,0xE4,0xE1)
#define BFF_RGB_MISTYROSE                BFF_RGB_MISTY_ROSE
#define BFF_RGB_MOCCASIN                 MAKE_RGB(0xFF,0xE4,0xB5)
#define BFF_RGB_NAVAJO_WHITE             MAKE_RGB(0xFF,0xDE,0xAD)
#define BFF_RGB_NAVAJOWHITE              BFF_RGB_NAVAJO_WHITE
#define BFF_RGB_NAVY                     MAKE_RGB(0x00,0x00,0x80)
#define BFF_RGB_OLD_LACE                 MAKE_RGB(0xFD,0xF5,0xE6)
#define BFF_RGB_OLDLACE                  BFF_RGB_OLD_LACE
#define BFF_RGB_OLIVE                    MAKE_RGB(0x80,0x80,0x00)
#define BFF_RGB_OLIVE_DRAB               MAKE_RGB(0x6B,0x8E,0x23)
#define BFF_RGB_OLIVEDRAB                BFF_RGB_OLIVE_DRAB
#define BFF_RGB_ORANGE                   MAKE_RGB(0xFF,0xA5,0x00)
#define BFF_RGB_ORANGE_RED               MAKE_RGB(0xFF,0x45,0x00)
#define BFF_RGB_ORANGERED                BFF_RGB_ORANGE_RED
#define BFF_RGB_ORCHID                   MAKE_RGB(0xDA,0x70,0xD6)
#define BFF_RGB_PALE_GOLDEN_ROD          MAKE_RGB(0xEE,0xE8,0xAA)
#define BFF_RGB_PALEGOLDENROD            BFF_RGB_PALE_GOLDEN_ROD
#define BFF_RGB_PALE_GREEN               MAKE_RGB(0x98,0xFB,0x98)
#define BFF_RGB_PALEGREEN                BFF_RGB_PALE_GREEN
#define BFF_RGB_PALE_TURQUOISE           MAKE_RGB(0xAF,0xEE,0xEE)
#define BFF_RGB_PALETURQUOISE            BFF_RGB_PALE_TURQUOISE
#define BFF_RGB_PALE_VIOLET_RED          MAKE_RGB(0xD8,0x70,0x93)
#define BFF_RGB_PALEVIOLETRED            BFF_RGB_PALE_VIOLET_RED
#define BFF_RGB_PAPAYA_WHIP              MAKE_RGB(0xFF,0xEF,0xD5)
#define BFF_RGB_PAPAYAWHIP               BFF_RGB_PAPAYA_WHIP
#define BFF_RGB_PEACH_PUFF               MAKE_RGB(0xFF,0xDA,0xB9)
#define BFF_RGB_PEACHPUFF                BFF_RGB_PEACH_PUFF
#define BFF_RGB_PERU                     MAKE_RGB(0xCD,0x85,0x3F)
#define BFF_RGB_PINK                     MAKE_RGB(0xFF,0xC0,0xCB)
#define BFF_RGB_PLUM                     MAKE_RGB(0xDD,0xA0,0xDD)
#define BFF_RGB_POWDER_BLUE              MAKE_RGB(0xB0,0xE0,0xE6)
#define BFF_RGB_POWDERBLUE               BFF_RGB_POWDER_BLUE
#define BFF_RGB_PURPLE                   MAKE_RGB(0x80,0x00,0x80)
#define BFF_RGB_RED                      MAKE_RGB(0xFF,0x00,0x00)
#define BFF_RGB_ROSY_BROWN               MAKE_RGB(0xBC,0x8F,0x8F)
#define BFF_RGB_ROSYBROWN                BFF_RGB_ROSY_BROWN
#define BFF_RGB_ROYAL_BLUE               MAKE_RGB(0x41,0x69,0xE1)
#define BFF_RGB_ROYALBLUE                BFF_RGB_ROYAL_BLUE
#define BFF_RGB_SADDLE_BROWN             MAKE_RGB(0x8B,0x45,0x13)
#define BFF_RGB_SADDLEBROWN              BFF_RGB_SADDLE_BROWN
#define BFF_RGB_SALMON                   MAKE_RGB(0xFA,0x80,0x72)
#define BFF_RGB_SANDY_BROWN              MAKE_RGB(0xF4,0xA4,0x60)
#define BFF_RGB_SANDYBROWN               BFF_RGB_SANDY_BROWN
#define BFF_RGB_SEA_GREEN                MAKE_RGB(0x2E,0x8B,0x57)
#define BFF_RGB_SEAGREEN                 BFF_RGB_SEA_GREEN
#define BFF_RGB_SEA_SHELL                MAKE_RGB(0xFF,0xF5,0xEE)
#define BFF_RGB_SEASHELL                 BFF_RGB_SEA_SHELL
#define BFF_RGB_SIENNA                   MAKE_RGB(0xA0,0x52,0x2D)
#define BFF_RGB_SILVER                   MAKE_RGB(0xC0,0xC0,0xC0)
#define BFF_RGB_SKY_BLUE                 MAKE_RGB(0x87,0xCE,0xEB)
#define BFF_RGB_SKYBLUE                  BFF_RGB_SKY_BLUE
#define BFF_RGB_SLATE_BLUE               MAKE_RGB(0x6A,0x5A,0xCD)
#define BFF_RGB_SLATEBLUE                BFF_RGB_SLATE_BLUE
#define BFF_RGB_SLATE_GRAY               MAKE_RGB(0x70,0x80,0x90)
#define BFF_RGB_SLATEGRAY                BFF_RGB_SLATE_GRAY
#define BFF_RGB_SLATE_GREY               BFF_RGB_SLATE_GRAY
#define BFF_RGB_SLATEGREY                BFF_RGB_SLATE_GREY
#define BFF_RGB_SNOW                     MAKE_RGB(0xFF,0xFA,0xFA)
#define BFF_RGB_SPRING_GREEN             MAKE_RGB(0x00,0xFF,0x7F)
#define BFF_RGB_SPRINGGREEN              BFF_RGB_SPRING_GREEN
#define BFF_RGB_STEEL_BLUE               MAKE_RGB(0x46,0x82,0xB4)
#define BFF_RGB_STEELBLUE                BFF_RGB_STEEL_BLUE
#define BFF_RGB_TAN                      MAKE_RGB(0xD2,0xB4,0x8C)
#define BFF_RGB_TEAL                     MAKE_RGB(0x00,0x80,0x80)
#define BFF_RGB_THISTLE                  MAKE_RGB(0xD8,0xBF,0xD8)
#define BFF_RGB_TOMATO                   MAKE_RGB(0xFF,0x63,0x47)
#define BFF_RGB_TURQUOISE                MAKE_RGB(0x40,0xE0,0xD0)
#define BFF_RGB_VIOLET                   MAKE_RGB(0xEE,0x82,0xEE)
#define BFF_RGB_WHEAT                    MAKE_RGB(0xF5,0xDE,0xB3)
#define BFF_RGB_WHITE                    MAKE_RGB(0xFF,0xFF,0xFF)
#define BFF_RGB_WHITE_SMOKE              MAKE_RGB(0xF5,0xF5,0xF5)
#define BFF_RGB_WHITESMOKE               BFF_RGB_WHITE_SMOKE
#define BFF_RGB_YELLOW                   MAKE_RGB(0xFF,0xFF,0x00)
#define BFF_RGB_YELLOW_GREEN             MAKE_RGB(0x9A,0xCD,0x32)
#define BFF_RGB_YELLOWGREEN              BFF_RGB_YELLOW_GREEN


#endif /* BFF_COLOR_H */
