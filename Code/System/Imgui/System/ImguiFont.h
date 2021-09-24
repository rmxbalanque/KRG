#pragma once
#include "../_Module/API.h"
#include "System/Core/Types/IntegralTypes.h"
#include "System/Imgui/ThirdParty/imgui/imgui.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

struct ImFont;
struct ImVec4;

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    enum class Font : uint8
    {
        ExtraSmall = 0, // Mostly for smaller icon size
        Small,
        SmallBold,
        Medium,
        MediumBold,
        Large,
        ExtraLarge,

        NumFonts,
        Default = Medium,
    };

    //-------------------------------------------------------------------------

    struct SystemFonts
    {
        static ImFont* GetFont( Font font ) { return s_fonts[(int32) font]; }

        static ImFont* s_fonts[(int32) Font::NumFonts];
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_IMGUI_API [[nodiscard]] ScopedFont
    {
    public:

        ScopedFont( Font font );
        ScopedFont( Font font, ImColor const& color );
        explicit ScopedFont( Font font, Color const& color ) : ScopedFont( font, (ImColor) IM_COL32( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a ) ) {}
        ~ScopedFont();

    private:

        bool m_colorApplied = false;
    };

    //-------------------------------------------------------------------------

    KRG_SYSTEM_IMGUI_API inline void PushFont( Font font ) 
    {
        ImGui::PushFont( SystemFonts::s_fonts[(int8) font] ); 
    }

    KRG_SYSTEM_IMGUI_API inline void PushFontAndColor( Font font, ImColor const& color ) 
    {
        ImGui::PushFont( SystemFonts::s_fonts[(int8) font] );
        ImGui::PushStyleColor( ImGuiCol_Text, color.Value );
    }
}
#endif

//-------------------------------------------------------------------------
// FontAwesome4 code points
//-------------------------------------------------------------------------
// From: https://github.com/juliettef/IconFontCppHeaders

#define KRG_ICON_MIN_FA 0xf000
#define KRG_ICON_MAX_FA 0xf2e0
#define KRG_ICON_GLASS "\xef\x80\x80"	// U+f000
#define KRG_ICON_MUSIC "\xef\x80\x81"	// U+f001
#define KRG_ICON_SEARCH "\xef\x80\x82"	// U+f002
#define KRG_ICON_ENVELOPE_O "\xef\x80\x83"	// U+f003
#define KRG_ICON_HEART "\xef\x80\x84"	// U+f004
#define KRG_ICON_STAR "\xef\x80\x85"	// U+f005
#define KRG_ICON_STAR_O "\xef\x80\x86"	// U+f006
#define KRG_ICON_USER "\xef\x80\x87"	// U+f007
#define KRG_ICON_FILM "\xef\x80\x88"	// U+f008
#define KRG_ICON_TH_LARGE "\xef\x80\x89"	// U+f009
#define KRG_ICON_TH "\xef\x80\x8a"	// U+f00a
#define KRG_ICON_TH_LIST "\xef\x80\x8b"	// U+f00b
#define KRG_ICON_CHECK "\xef\x80\x8c"	// U+f00c
#define KRG_ICON_TIMES "\xef\x80\x8d"	// U+f00d
#define KRG_ICON_SEARCH_PLUS "\xef\x80\x8e"	// U+f00e
#define KRG_ICON_SEARCH_MINUS "\xef\x80\x90"	// U+f010
#define KRG_ICON_POWER_OFF "\xef\x80\x91"	// U+f011
#define KRG_ICON_SIGNAL "\xef\x80\x92"	// U+f012
#define KRG_ICON_COG "\xef\x80\x93"	// U+f013
#define KRG_ICON_TRASH_O "\xef\x80\x94"	// U+f014
#define KRG_ICON_HOME "\xef\x80\x95"	// U+f015
#define KRG_ICON_FILE_O "\xef\x80\x96"	// U+f016
#define KRG_ICON_CLOCK_O "\xef\x80\x97"	// U+f017
#define KRG_ICON_ROAD "\xef\x80\x98"	// U+f018
#define KRG_ICON_DOWNLOAD "\xef\x80\x99"	// U+f019
#define KRG_ICON_ARROW_CIRCLE_O_DOWN "\xef\x80\x9a"	// U+f01a
#define KRG_ICON_ARROW_CIRCLE_O_UP "\xef\x80\x9b"	// U+f01b
#define KRG_ICON_INBOX "\xef\x80\x9c"	// U+f01c
#define KRG_ICON_PLAY_CIRCLE_O "\xef\x80\x9d"	// U+f01d
#define KRG_ICON_REPEAT "\xef\x80\x9e"	// U+f01e
#define KRG_ICON_REFRESH "\xef\x80\xa1"	// U+f021
#define KRG_ICON_LIST_ALT "\xef\x80\xa2"	// U+f022
#define KRG_ICON_LOCK "\xef\x80\xa3"	// U+f023
#define KRG_ICON_FLAG "\xef\x80\xa4"	// U+f024
#define KRG_ICON_HEADPHONES "\xef\x80\xa5"	// U+f025
#define KRG_ICON_VOLUME_OFF "\xef\x80\xa6"	// U+f026
#define KRG_ICON_VOLUME_DOWN "\xef\x80\xa7"	// U+f027
#define KRG_ICON_VOLUME_UP "\xef\x80\xa8"	// U+f028
#define KRG_ICON_QRCODE "\xef\x80\xa9"	// U+f029
#define KRG_ICON_BARCODE "\xef\x80\xaa"	// U+f02a
#define KRG_ICON_TAG "\xef\x80\xab"	// U+f02b
#define KRG_ICON_TAGS "\xef\x80\xac"	// U+f02c
#define KRG_ICON_BOOK "\xef\x80\xad"	// U+f02d
#define KRG_ICON_BOOKMARK "\xef\x80\xae"	// U+f02e
#define KRG_ICON_PRINT "\xef\x80\xaf"	// U+f02f
#define KRG_ICON_CAMERA "\xef\x80\xb0"	// U+f030
#define KRG_ICON_FONT "\xef\x80\xb1"	// U+f031
#define KRG_ICON_BOLD "\xef\x80\xb2"	// U+f032
#define KRG_ICON_ITALIC "\xef\x80\xb3"	// U+f033
#define KRG_ICON_TEXT_HEIGHT "\xef\x80\xb4"	// U+f034
#define KRG_ICON_TEXT_WIDTH "\xef\x80\xb5"	// U+f035
#define KRG_ICON_ALIGN_LEFT "\xef\x80\xb6"	// U+f036
#define KRG_ICON_ALIGN_CENTER "\xef\x80\xb7"	// U+f037
#define KRG_ICON_ALIGN_RIGHT "\xef\x80\xb8"	// U+f038
#define KRG_ICON_ALIGN_JUSTIFY "\xef\x80\xb9"	// U+f039
#define KRG_ICON_LIST "\xef\x80\xba"	// U+f03a
#define KRG_ICON_OUTDENT "\xef\x80\xbb"	// U+f03b
#define KRG_ICON_INDENT "\xef\x80\xbc"	// U+f03c
#define KRG_ICON_VIDEO_CAMERA "\xef\x80\xbd"	// U+f03d
#define KRG_ICON_PICTURE_O "\xef\x80\xbe"	// U+f03e
#define KRG_ICON_PENCIL "\xef\x81\x80"	// U+f040
#define KRG_ICON_MAP_MARKER "\xef\x81\x81"	// U+f041
#define KRG_ICON_ADJUST "\xef\x81\x82"	// U+f042
#define KRG_ICON_TINT "\xef\x81\x83"	// U+f043
#define KRG_ICON_PENCIL_SQUARE_O "\xef\x81\x84"	// U+f044
#define KRG_ICON_SHARE_SQUARE_O "\xef\x81\x85"	// U+f045
#define KRG_ICON_CHECK_SQUARE_O "\xef\x81\x86"	// U+f046
#define KRG_ICON_ARROWS "\xef\x81\x87"	// U+f047
#define KRG_ICON_STEP_BACKWARD "\xef\x81\x88"	// U+f048
#define KRG_ICON_FAST_BACKWARD "\xef\x81\x89"	// U+f049
#define KRG_ICON_BACKWARD "\xef\x81\x8a"	// U+f04a
#define KRG_ICON_PLAY "\xef\x81\x8b"	// U+f04b
#define KRG_ICON_PAUSE "\xef\x81\x8c"	// U+f04c
#define KRG_ICON_STOP "\xef\x81\x8d"	// U+f04d
#define KRG_ICON_FORWARD "\xef\x81\x8e"	// U+f04e
#define KRG_ICON_FAST_FORWARD "\xef\x81\x90"	// U+f050
#define KRG_ICON_STEP_FORWARD "\xef\x81\x91"	// U+f051
#define KRG_ICON_EJECT "\xef\x81\x92"	// U+f052
#define KRG_ICON_CHEVRON_LEFT "\xef\x81\x93"	// U+f053
#define KRG_ICON_CHEVRON_RIGHT "\xef\x81\x94"	// U+f054
#define KRG_ICON_PLUS_CIRCLE "\xef\x81\x95"	// U+f055
#define KRG_ICON_MINUS_CIRCLE "\xef\x81\x96"	// U+f056
#define KRG_ICON_TIMES_CIRCLE "\xef\x81\x97"	// U+f057
#define KRG_ICON_CHECK_CIRCLE "\xef\x81\x98"	// U+f058
#define KRG_ICON_QUESTION_CIRCLE "\xef\x81\x99"	// U+f059
#define KRG_ICON_INFO_CIRCLE "\xef\x81\x9a"	// U+f05a
#define KRG_ICON_CROSSHAIRS "\xef\x81\x9b"	// U+f05b
#define KRG_ICON_TIMES_CIRCLE_O "\xef\x81\x9c"	// U+f05c
#define KRG_ICON_CHECK_CIRCLE_O "\xef\x81\x9d"	// U+f05d
#define KRG_ICON_BAN "\xef\x81\x9e"	// U+f05e
#define KRG_ICON_ARROW_LEFT "\xef\x81\xa0"	// U+f060
#define KRG_ICON_ARROW_RIGHT "\xef\x81\xa1"	// U+f061
#define KRG_ICON_ARROW_UP "\xef\x81\xa2"	// U+f062
#define KRG_ICON_ARROW_DOWN "\xef\x81\xa3"	// U+f063
#define KRG_ICON_SHARE "\xef\x81\xa4"	// U+f064
#define KRG_ICON_EXPAND "\xef\x81\xa5"	// U+f065
#define KRG_ICON_COMPRESS "\xef\x81\xa6"	// U+f066
#define KRG_ICON_PLUS "\xef\x81\xa7"	// U+f067
#define KRG_ICON_MINUS "\xef\x81\xa8"	// U+f068
#define KRG_ICON_ASTERISK "\xef\x81\xa9"	// U+f069
#define KRG_ICON_EXCLAMATION_CIRCLE "\xef\x81\xaa"	// U+f06a
#define KRG_ICON_GIFT "\xef\x81\xab"	// U+f06b
#define KRG_ICON_LEAF "\xef\x81\xac"	// U+f06c
#define KRG_ICON_FIRE "\xef\x81\xad"	// U+f06d
#define KRG_ICON_EYE "\xef\x81\xae"	// U+f06e
#define KRG_ICON_EYE_SLASH "\xef\x81\xb0"	// U+f070
#define KRG_ICON_EXCLAMATION_TRIANGLE "\xef\x81\xb1"	// U+f071
#define KRG_ICON_PLANE "\xef\x81\xb2"	// U+f072
#define KRG_ICON_CALENDAR "\xef\x81\xb3"	// U+f073
#define KRG_ICON_RANDOM "\xef\x81\xb4"	// U+f074
#define KRG_ICON_COMMENT "\xef\x81\xb5"	// U+f075
#define KRG_ICON_MAGNET "\xef\x81\xb6"	// U+f076
#define KRG_ICON_CHEVRON_UP "\xef\x81\xb7"	// U+f077
#define KRG_ICON_CHEVRON_DOWN "\xef\x81\xb8"	// U+f078
#define KRG_ICON_RETWEET "\xef\x81\xb9"	// U+f079
#define KRG_ICON_SHOPPING_CART "\xef\x81\xba"	// U+f07a
#define KRG_ICON_FOLDER "\xef\x81\xbb"	// U+f07b
#define KRG_ICON_FOLDER_OPEN "\xef\x81\xbc"	// U+f07c
#define KRG_ICON_ARROWS_V "\xef\x81\xbd"	// U+f07d
#define KRG_ICON_ARROWS_H "\xef\x81\xbe"	// U+f07e
#define KRG_ICON_BAR_CHART "\xef\x82\x80"	// U+f080
#define KRG_ICON_TWITTER_SQUARE "\xef\x82\x81"	// U+f081
#define KRG_ICON_FACEBOOK_SQUARE "\xef\x82\x82"	// U+f082
#define KRG_ICON_CAMERA_RETRO "\xef\x82\x83"	// U+f083
#define KRG_ICON_KEY "\xef\x82\x84"	// U+f084
#define KRG_ICON_COGS "\xef\x82\x85"	// U+f085
#define KRG_ICON_COMMENTS "\xef\x82\x86"	// U+f086
#define KRG_ICON_THUMBS_O_UP "\xef\x82\x87"	// U+f087
#define KRG_ICON_THUMBS_O_DOWN "\xef\x82\x88"	// U+f088
#define KRG_ICON_STAR_HALF "\xef\x82\x89"	// U+f089
#define KRG_ICON_HEART_O "\xef\x82\x8a"	// U+f08a
#define KRG_ICON_SIGN_OUT "\xef\x82\x8b"	// U+f08b
#define KRG_ICON_LINKEDIN_SQUARE "\xef\x82\x8c"	// U+f08c
#define KRG_ICON_THUMB_TACK "\xef\x82\x8d"	// U+f08d
#define KRG_ICON_EXTERNAL_LINK "\xef\x82\x8e"	// U+f08e
#define KRG_ICON_SIGN_IN "\xef\x82\x90"	// U+f090
#define KRG_ICON_TROPHY "\xef\x82\x91"	// U+f091
#define KRG_ICON_GITHUB_SQUARE "\xef\x82\x92"	// U+f092
#define KRG_ICON_UPLOAD "\xef\x82\x93"	// U+f093
#define KRG_ICON_LEMON_O "\xef\x82\x94"	// U+f094
#define KRG_ICON_PHONE "\xef\x82\x95"	// U+f095
#define KRG_ICON_SQUARE_O "\xef\x82\x96"	// U+f096
#define KRG_ICON_BOOKMARK_O "\xef\x82\x97"	// U+f097
#define KRG_ICON_PHONE_SQUARE "\xef\x82\x98"	// U+f098
#define KRG_ICON_TWITTER "\xef\x82\x99"	// U+f099
#define KRG_ICON_FACEBOOK "\xef\x82\x9a"	// U+f09a
#define KRG_ICON_GITHUB "\xef\x82\x9b"	// U+f09b
#define KRG_ICON_UNLOCK "\xef\x82\x9c"	// U+f09c
#define KRG_ICON_CREDIT_CARD "\xef\x82\x9d"	// U+f09d
#define KRG_ICON_RSS "\xef\x82\x9e"	// U+f09e
#define KRG_ICON_HDD_O "\xef\x82\xa0"	// U+f0a0
#define KRG_ICON_BULLHORN "\xef\x82\xa1"	// U+f0a1
#define KRG_ICON_BELL "\xef\x83\xb3"	// U+f0f3
#define KRG_ICON_CERTIFICATE "\xef\x82\xa3"	// U+f0a3
#define KRG_ICON_HAND_O_RIGHT "\xef\x82\xa4"	// U+f0a4
#define KRG_ICON_HAND_O_LEFT "\xef\x82\xa5"	// U+f0a5
#define KRG_ICON_HAND_O_UP "\xef\x82\xa6"	// U+f0a6
#define KRG_ICON_HAND_O_DOWN "\xef\x82\xa7"	// U+f0a7
#define KRG_ICON_ARROW_CIRCLE_LEFT "\xef\x82\xa8"	// U+f0a8
#define KRG_ICON_ARROW_CIRCLE_RIGHT "\xef\x82\xa9"	// U+f0a9
#define KRG_ICON_ARROW_CIRCLE_UP "\xef\x82\xaa"	// U+f0aa
#define KRG_ICON_ARROW_CIRCLE_DOWN "\xef\x82\xab"	// U+f0ab
#define KRG_ICON_GLOBE "\xef\x82\xac"	// U+f0ac
#define KRG_ICON_WRENCH "\xef\x82\xad"	// U+f0ad
#define KRG_ICON_TASKS "\xef\x82\xae"	// U+f0ae
#define KRG_ICON_FILTER "\xef\x82\xb0"	// U+f0b0
#define KRG_ICON_BRIEFCASE "\xef\x82\xb1"	// U+f0b1
#define KRG_ICON_ARROWS_ALT "\xef\x82\xb2"	// U+f0b2
#define KRG_ICON_USERS "\xef\x83\x80"	// U+f0c0
#define KRG_ICON_LINK "\xef\x83\x81"	// U+f0c1
#define KRG_ICON_CLOUD "\xef\x83\x82"	// U+f0c2
#define KRG_ICON_FLASK "\xef\x83\x83"	// U+f0c3
#define KRG_ICON_SCISSORS "\xef\x83\x84"	// U+f0c4
#define KRG_ICON_FILES_O "\xef\x83\x85"	// U+f0c5
#define KRG_ICON_PAPERCLIP "\xef\x83\x86"	// U+f0c6
#define KRG_ICON_FLOPPY_O "\xef\x83\x87"	// U+f0c7
#define KRG_ICON_SQUARE "\xef\x83\x88"	// U+f0c8
#define KRG_ICON_BARS "\xef\x83\x89"	// U+f0c9
#define KRG_ICON_LIST_UL "\xef\x83\x8a"	// U+f0ca
#define KRG_ICON_LIST_OL "\xef\x83\x8b"	// U+f0cb
#define KRG_ICON_STRIKETHROUGH "\xef\x83\x8c"	// U+f0cc
#define KRG_ICON_UNDERLINE "\xef\x83\x8d"	// U+f0cd
#define KRG_ICON_TABLE "\xef\x83\x8e"	// U+f0ce
#define KRG_ICON_MAGIC "\xef\x83\x90"	// U+f0d0
#define KRG_ICON_TRUCK "\xef\x83\x91"	// U+f0d1
#define KRG_ICON_PINTEREST "\xef\x83\x92"	// U+f0d2
#define KRG_ICON_PINTEREST_SQUARE "\xef\x83\x93"	// U+f0d3
#define KRG_ICON_GOOGLE_PLUS_SQUARE "\xef\x83\x94"	// U+f0d4
#define KRG_ICON_GOOGLE_PLUS "\xef\x83\x95"	// U+f0d5
#define KRG_ICON_MONEY "\xef\x83\x96"	// U+f0d6
#define KRG_ICON_CARET_DOWN "\xef\x83\x97"	// U+f0d7
#define KRG_ICON_CARET_UP "\xef\x83\x98"	// U+f0d8
#define KRG_ICON_CARET_LEFT "\xef\x83\x99"	// U+f0d9
#define KRG_ICON_CARET_RIGHT "\xef\x83\x9a"	// U+f0da
#define KRG_ICON_COLUMNS "\xef\x83\x9b"	// U+f0db
#define KRG_ICON_SORT "\xef\x83\x9c"	// U+f0dc
#define KRG_ICON_SORT_DESC "\xef\x83\x9d"	// U+f0dd
#define KRG_ICON_SORT_ASC "\xef\x83\x9e"	// U+f0de
#define KRG_ICON_ENVELOPE "\xef\x83\xa0"	// U+f0e0
#define KRG_ICON_LINKEDIN "\xef\x83\xa1"	// U+f0e1
#define KRG_ICON_UNDO "\xef\x83\xa2"	// U+f0e2
#define KRG_ICON_GAVEL "\xef\x83\xa3"	// U+f0e3
#define KRG_ICON_TACHOMETER "\xef\x83\xa4"	// U+f0e4
#define KRG_ICON_COMMENT_O "\xef\x83\xa5"	// U+f0e5
#define KRG_ICON_COMMENTS_O "\xef\x83\xa6"	// U+f0e6
#define KRG_ICON_BOLT "\xef\x83\xa7"	// U+f0e7
#define KRG_ICON_SITEMAP "\xef\x83\xa8"	// U+f0e8
#define KRG_ICON_UMBRELLA "\xef\x83\xa9"	// U+f0e9
#define KRG_ICON_CLIPBOARD "\xef\x83\xaa"	// U+f0ea
#define KRG_ICON_LIGHTBULB_O "\xef\x83\xab"	// U+f0eb
#define KRG_ICON_EXCHANGE "\xef\x83\xac"	// U+f0ec
#define KRG_ICON_CLOUD_DOWNLOAD "\xef\x83\xad"	// U+f0ed
#define KRG_ICON_CLOUD_UPLOAD "\xef\x83\xae"	// U+f0ee
#define KRG_ICON_USER_MD "\xef\x83\xb0"	// U+f0f0
#define KRG_ICON_STETHOSCOPE "\xef\x83\xb1"	// U+f0f1
#define KRG_ICON_SUITCASE "\xef\x83\xb2"	// U+f0f2
#define KRG_ICON_BELL_O "\xef\x82\xa2"	// U+f0a2
#define KRG_ICON_COFFEE "\xef\x83\xb4"	// U+f0f4
#define KRG_ICON_CUTLERY "\xef\x83\xb5"	// U+f0f5
#define KRG_ICON_FILE_TEXT_O "\xef\x83\xb6"	// U+f0f6
#define KRG_ICON_BUILDING_O "\xef\x83\xb7"	// U+f0f7
#define KRG_ICON_HOSPITAL_O "\xef\x83\xb8"	// U+f0f8
#define KRG_ICON_AMBULANCE "\xef\x83\xb9"	// U+f0f9
#define KRG_ICON_MEDKIT "\xef\x83\xba"	// U+f0fa
#define KRG_ICON_FIGHTER_JET "\xef\x83\xbb"	// U+f0fb
#define KRG_ICON_BEER "\xef\x83\xbc"	// U+f0fc
#define KRG_ICON_H_SQUARE "\xef\x83\xbd"	// U+f0fd
#define KRG_ICON_PLUS_SQUARE "\xef\x83\xbe"	// U+f0fe
#define KRG_ICON_ANGLE_DOUBLE_LEFT "\xef\x84\x80"	// U+f100
#define KRG_ICON_ANGLE_DOUBLE_RIGHT "\xef\x84\x81"	// U+f101
#define KRG_ICON_ANGLE_DOUBLE_UP "\xef\x84\x82"	// U+f102
#define KRG_ICON_ANGLE_DOUBLE_DOWN "\xef\x84\x83"	// U+f103
#define KRG_ICON_ANGLE_LEFT "\xef\x84\x84"	// U+f104
#define KRG_ICON_ANGLE_RIGHT "\xef\x84\x85"	// U+f105
#define KRG_ICON_ANGLE_UP "\xef\x84\x86"	// U+f106
#define KRG_ICON_ANGLE_DOWN "\xef\x84\x87"	// U+f107
#define KRG_ICON_DESKTOP "\xef\x84\x88"	// U+f108
#define KRG_ICON_LAPTOP "\xef\x84\x89"	// U+f109
#define KRG_ICON_TABLET "\xef\x84\x8a"	// U+f10a
#define KRG_ICON_MOBILE "\xef\x84\x8b"	// U+f10b
#define KRG_ICON_CIRCLE_O "\xef\x84\x8c"	// U+f10c
#define KRG_ICON_QUOTE_LEFT "\xef\x84\x8d"	// U+f10d
#define KRG_ICON_QUOTE_RIGHT "\xef\x84\x8e"	// U+f10e
#define KRG_ICON_SPINNER "\xef\x84\x90"	// U+f110
#define KRG_ICON_CIRCLE "\xef\x84\x91"	// U+f111
#define KRG_ICON_REPLY "\xef\x84\x92"	// U+f112
#define KRG_ICON_GITHUB_ALT "\xef\x84\x93"	// U+f113
#define KRG_ICON_FOLDER_O "\xef\x84\x94"	// U+f114
#define KRG_ICON_FOLDER_OPEN_O "\xef\x84\x95"	// U+f115
#define KRG_ICON_SMILE_O "\xef\x84\x98"	// U+f118
#define KRG_ICON_FROWN_O "\xef\x84\x99"	// U+f119
#define KRG_ICON_MEH_O "\xef\x84\x9a"	// U+f11a
#define KRG_ICON_GAMEPAD "\xef\x84\x9b"	// U+f11b
#define KRG_ICON_KEYBOARD_O "\xef\x84\x9c"	// U+f11c
#define KRG_ICON_FLAG_O "\xef\x84\x9d"	// U+f11d
#define KRG_ICON_FLAG_CHECKERED "\xef\x84\x9e"	// U+f11e
#define KRG_ICON_TERMINAL "\xef\x84\xa0"	// U+f120
#define KRG_ICON_CODE "\xef\x84\xa1"	// U+f121
#define KRG_ICON_REPLY_ALL "\xef\x84\xa2"	// U+f122
#define KRG_ICON_STAR_HALF_O "\xef\x84\xa3"	// U+f123
#define KRG_ICON_LOCATION_ARROW "\xef\x84\xa4"	// U+f124
#define KRG_ICON_CROP "\xef\x84\xa5"	// U+f125
#define KRG_ICON_CODE_FORK "\xef\x84\xa6"	// U+f126
#define KRG_ICON_CHAIN_BROKEN "\xef\x84\xa7"	// U+f127
#define KRG_ICON_QUESTION "\xef\x84\xa8"	// U+f128
#define KRG_ICON_INFO "\xef\x84\xa9"	// U+f129
#define KRG_ICON_EXCLAMATION "\xef\x84\xaa"	// U+f12a
#define KRG_ICON_SUPERSCRIPT "\xef\x84\xab"	// U+f12b
#define KRG_ICON_SUBSCRIPT "\xef\x84\xac"	// U+f12c
#define KRG_ICON_ERASER "\xef\x84\xad"	// U+f12d
#define KRG_ICON_PUZZLE_PIECE "\xef\x84\xae"	// U+f12e
#define KRG_ICON_MICROPHONE "\xef\x84\xb0"	// U+f130
#define KRG_ICON_MICROPHONE_SLASH "\xef\x84\xb1"	// U+f131
#define KRG_ICON_SHIELD "\xef\x84\xb2"	// U+f132
#define KRG_ICON_CALENDAR_O "\xef\x84\xb3"	// U+f133
#define KRG_ICON_FIRE_EXTINGUISHER "\xef\x84\xb4"	// U+f134
#define KRG_ICON_ROCKET "\xef\x84\xb5"	// U+f135
#define KRG_ICON_MAXCDN "\xef\x84\xb6"	// U+f136
#define KRG_ICON_CHEVRON_CIRCLE_LEFT "\xef\x84\xb7"	// U+f137
#define KRG_ICON_CHEVRON_CIRCLE_RIGHT "\xef\x84\xb8"	// U+f138
#define KRG_ICON_CHEVRON_CIRCLE_UP "\xef\x84\xb9"	// U+f139
#define KRG_ICON_CHEVRON_CIRCLE_DOWN "\xef\x84\xba"	// U+f13a
#define KRG_ICON_HTML5 "\xef\x84\xbb"	// U+f13b
#define KRG_ICON_CSS3 "\xef\x84\xbc"	// U+f13c
#define KRG_ICON_ANCHOR "\xef\x84\xbd"	// U+f13d
#define KRG_ICON_UNLOCK_ALT "\xef\x84\xbe"	// U+f13e
#define KRG_ICON_BULLSEYE "\xef\x85\x80"	// U+f140
#define KRG_ICON_ELLIPSIS_H "\xef\x85\x81"	// U+f141
#define KRG_ICON_ELLIPSIS_V "\xef\x85\x82"	// U+f142
#define KRG_ICON_RSS_SQUARE "\xef\x85\x83"	// U+f143
#define KRG_ICON_PLAY_CIRCLE "\xef\x85\x84"	// U+f144
#define KRG_ICON_TICKET "\xef\x85\x85"	// U+f145
#define KRG_ICON_MINUS_SQUARE "\xef\x85\x86"	// U+f146
#define KRG_ICON_MINUS_SQUARE_O "\xef\x85\x87"	// U+f147
#define KRG_ICON_LEVEL_UP "\xef\x85\x88"	// U+f148
#define KRG_ICON_LEVEL_DOWN "\xef\x85\x89"	// U+f149
#define KRG_ICON_CHECK_SQUARE "\xef\x85\x8a"	// U+f14a
#define KRG_ICON_PENCIL_SQUARE "\xef\x85\x8b"	// U+f14b
#define KRG_ICON_EXTERNAL_LINK_SQUARE "\xef\x85\x8c"	// U+f14c
#define KRG_ICON_SHARE_SQUARE "\xef\x85\x8d"	// U+f14d
#define KRG_ICON_COMPASS "\xef\x85\x8e"	// U+f14e
#define KRG_ICON_CARET_SQUARE_O_DOWN "\xef\x85\x90"	// U+f150
#define KRG_ICON_CARET_SQUARE_O_UP "\xef\x85\x91"	// U+f151
#define KRG_ICON_CARET_SQUARE_O_RIGHT "\xef\x85\x92"	// U+f152
#define KRG_ICON_EUR "\xef\x85\x93"	// U+f153
#define KRG_ICON_GBP "\xef\x85\x94"	// U+f154
#define KRG_ICON_USD "\xef\x85\x95"	// U+f155
#define KRG_ICON_INR "\xef\x85\x96"	// U+f156
#define KRG_ICON_JPY "\xef\x85\x97"	// U+f157
#define KRG_ICON_RUB "\xef\x85\x98"	// U+f158
#define KRG_ICON_KRW "\xef\x85\x99"	// U+f159
#define KRG_ICON_BTC "\xef\x85\x9a"	// U+f15a
#define KRG_ICON_FILE "\xef\x85\x9b"	// U+f15b
#define KRG_ICON_FILE_TEXT "\xef\x85\x9c"	// U+f15c
#define KRG_ICON_SORT_ALPHA_ASC "\xef\x85\x9d"	// U+f15d
#define KRG_ICON_SORT_ALPHA_DESC "\xef\x85\x9e"	// U+f15e
#define KRG_ICON_SORT_AMOUNT_ASC "\xef\x85\xa0"	// U+f160
#define KRG_ICON_SORT_AMOUNT_DESC "\xef\x85\xa1"	// U+f161
#define KRG_ICON_SORT_NUMERIC_ASC "\xef\x85\xa2"	// U+f162
#define KRG_ICON_SORT_NUMERIC_DESC "\xef\x85\xa3"	// U+f163
#define KRG_ICON_THUMBS_UP "\xef\x85\xa4"	// U+f164
#define KRG_ICON_THUMBS_DOWN "\xef\x85\xa5"	// U+f165
#define KRG_ICON_YOUTUBE_SQUARE "\xef\x85\xa6"	// U+f166
#define KRG_ICON_YOUTUBE "\xef\x85\xa7"	// U+f167
#define KRG_ICON_XING "\xef\x85\xa8"	// U+f168
#define KRG_ICON_XING_SQUARE "\xef\x85\xa9"	// U+f169
#define KRG_ICON_YOUTUBE_PLAY "\xef\x85\xaa"	// U+f16a
#define KRG_ICON_DROPBOX "\xef\x85\xab"	// U+f16b
#define KRG_ICON_STACK_OVERFLOW "\xef\x85\xac"	// U+f16c
#define KRG_ICON_INSTAGRAM "\xef\x85\xad"	// U+f16d
#define KRG_ICON_FLICKR "\xef\x85\xae"	// U+f16e
#define KRG_ICON_ADN "\xef\x85\xb0"	// U+f170
#define KRG_ICON_BITBUCKET "\xef\x85\xb1"	// U+f171
#define KRG_ICON_BITBUCKET_SQUARE "\xef\x85\xb2"	// U+f172
#define KRG_ICON_TUMBLR "\xef\x85\xb3"	// U+f173
#define KRG_ICON_TUMBLR_SQUARE "\xef\x85\xb4"	// U+f174
#define KRG_ICON_LONG_ARROW_DOWN "\xef\x85\xb5"	// U+f175
#define KRG_ICON_LONG_ARROW_UP "\xef\x85\xb6"	// U+f176
#define KRG_ICON_LONG_ARROW_LEFT "\xef\x85\xb7"	// U+f177
#define KRG_ICON_LONG_ARROW_RIGHT "\xef\x85\xb8"	// U+f178
#define KRG_ICON_APPLE "\xef\x85\xb9"	// U+f179
#define KRG_ICON_WINDOWS "\xef\x85\xba"	// U+f17a
#define KRG_ICON_ANDROID "\xef\x85\xbb"	// U+f17b
#define KRG_ICON_LINUX "\xef\x85\xbc"	// U+f17c
#define KRG_ICON_DRIBBBLE "\xef\x85\xbd"	// U+f17d
#define KRG_ICON_SKYPE "\xef\x85\xbe"	// U+f17e
#define KRG_ICON_FOURSQUARE "\xef\x86\x80"	// U+f180
#define KRG_ICON_TRELLO "\xef\x86\x81"	// U+f181
#define KRG_ICON_FEMALE "\xef\x86\x82"	// U+f182
#define KRG_ICON_MALE "\xef\x86\x83"	// U+f183
#define KRG_ICON_GRATIPAY "\xef\x86\x84"	// U+f184
#define KRG_ICON_SUN_O "\xef\x86\x85"	// U+f185
#define KRG_ICON_MOON_O "\xef\x86\x86"	// U+f186
#define KRG_ICON_ARCHIVE "\xef\x86\x87"	// U+f187
#define KRG_ICON_BUG "\xef\x86\x88"	// U+f188
#define KRG_ICON_VK "\xef\x86\x89"	// U+f189
#define KRG_ICON_WEIBO "\xef\x86\x8a"	// U+f18a
#define KRG_ICON_RENREN "\xef\x86\x8b"	// U+f18b
#define KRG_ICON_PAGELINES "\xef\x86\x8c"	// U+f18c
#define KRG_ICON_STACK_EXCHANGE "\xef\x86\x8d"	// U+f18d
#define KRG_ICON_ARROW_CIRCLE_O_RIGHT "\xef\x86\x8e"	// U+f18e
#define KRG_ICON_ARROW_CIRCLE_O_LEFT "\xef\x86\x90"	// U+f190
#define KRG_ICON_CARET_SQUARE_O_LEFT "\xef\x86\x91"	// U+f191
#define KRG_ICON_DOT_CIRCLE_O "\xef\x86\x92"	// U+f192
#define KRG_ICON_WHEELCHAIR "\xef\x86\x93"	// U+f193
#define KRG_ICON_VIMEO_SQUARE "\xef\x86\x94"	// U+f194
#define KRG_ICON_TRY "\xef\x86\x95"	// U+f195
#define KRG_ICON_PLUS_SQUARE_O "\xef\x86\x96"	// U+f196
#define KRG_ICON_SPACE_SHUTTLE "\xef\x86\x97"	// U+f197
#define KRG_ICON_SLACK "\xef\x86\x98"	// U+f198
#define KRG_ICON_ENVELOPE_SQUARE "\xef\x86\x99"	// U+f199
#define KRG_ICON_WORDPRESS "\xef\x86\x9a"	// U+f19a
#define KRG_ICON_OPENID "\xef\x86\x9b"	// U+f19b
#define KRG_ICON_UNIVERSITY "\xef\x86\x9c"	// U+f19c
#define KRG_ICON_GRADUATION_CAP "\xef\x86\x9d"	// U+f19d
#define KRG_ICON_YAHOO "\xef\x86\x9e"	// U+f19e
#define KRG_ICON_GOOGLE "\xef\x86\xa0"	// U+f1a0
#define KRG_ICON_REDDIT "\xef\x86\xa1"	// U+f1a1
#define KRG_ICON_REDDIT_SQUARE "\xef\x86\xa2"	// U+f1a2
#define KRG_ICON_STUMBLEUPON_CIRCLE "\xef\x86\xa3"	// U+f1a3
#define KRG_ICON_STUMBLEUPON "\xef\x86\xa4"	// U+f1a4
#define KRG_ICON_DELICIOUS "\xef\x86\xa5"	// U+f1a5
#define KRG_ICON_DIGG "\xef\x86\xa6"	// U+f1a6
#define KRG_ICON_PIED_PIPER_PP "\xef\x86\xa7"	// U+f1a7
#define KRG_ICON_PIED_PIPER_ALT "\xef\x86\xa8"	// U+f1a8
#define KRG_ICON_DRUPAL "\xef\x86\xa9"	// U+f1a9
#define KRG_ICON_JOOMLA "\xef\x86\xaa"	// U+f1aa
#define KRG_ICON_LANGUAGE "\xef\x86\xab"	// U+f1ab
#define KRG_ICON_FAX "\xef\x86\xac"	// U+f1ac
#define KRG_ICON_BUILDING "\xef\x86\xad"	// U+f1ad
#define KRG_ICON_CHILD "\xef\x86\xae"	// U+f1ae
#define KRG_ICON_PAW "\xef\x86\xb0"	// U+f1b0
#define KRG_ICON_SPOON "\xef\x86\xb1"	// U+f1b1
#define KRG_ICON_CUBE "\xef\x86\xb2"	// U+f1b2
#define KRG_ICON_CUBES "\xef\x86\xb3"	// U+f1b3
#define KRG_ICON_BEHANCE "\xef\x86\xb4"	// U+f1b4
#define KRG_ICON_BEHANCE_SQUARE "\xef\x86\xb5"	// U+f1b5
#define KRG_ICON_STEAM "\xef\x86\xb6"	// U+f1b6
#define KRG_ICON_STEAM_SQUARE "\xef\x86\xb7"	// U+f1b7
#define KRG_ICON_RECYCLE "\xef\x86\xb8"	// U+f1b8
#define KRG_ICON_CAR "\xef\x86\xb9"	// U+f1b9
#define KRG_ICON_TAXI "\xef\x86\xba"	// U+f1ba
#define KRG_ICON_TREE "\xef\x86\xbb"	// U+f1bb
#define KRG_ICON_SPOTIFY "\xef\x86\xbc"	// U+f1bc
#define KRG_ICON_DEVIANTART "\xef\x86\xbd"	// U+f1bd
#define KRG_ICON_SOUNDCLOUD "\xef\x86\xbe"	// U+f1be
#define KRG_ICON_DATABASE "\xef\x87\x80"	// U+f1c0
#define KRG_ICON_FILE_PDF_O "\xef\x87\x81"	// U+f1c1
#define KRG_ICON_FILE_WORD_O "\xef\x87\x82"	// U+f1c2
#define KRG_ICON_FILE_EXCEL_O "\xef\x87\x83"	// U+f1c3
#define KRG_ICON_FILE_POWERPOINT_O "\xef\x87\x84"	// U+f1c4
#define KRG_ICON_FILE_IMAGE_O "\xef\x87\x85"	// U+f1c5
#define KRG_ICON_FILE_ARCHIVE_O "\xef\x87\x86"	// U+f1c6
#define KRG_ICON_FILE_AUDIO_O "\xef\x87\x87"	// U+f1c7
#define KRG_ICON_FILE_VIDEO_O "\xef\x87\x88"	// U+f1c8
#define KRG_ICON_FILE_CODE_O "\xef\x87\x89"	// U+f1c9
#define KRG_ICON_VINE "\xef\x87\x8a"	// U+f1ca
#define KRG_ICON_CODEPEN "\xef\x87\x8b"	// U+f1cb
#define KRG_ICON_JSFIDDLE "\xef\x87\x8c"	// U+f1cc
#define KRG_ICON_LIFE_RING "\xef\x87\x8d"	// U+f1cd
#define KRG_ICON_CIRCLE_O_NOTCH "\xef\x87\x8e"	// U+f1ce
#define KRG_ICON_REBEL "\xef\x87\x90"	// U+f1d0
#define KRG_ICON_EMPIRE "\xef\x87\x91"	// U+f1d1
#define KRG_ICON_GIT_SQUARE "\xef\x87\x92"	// U+f1d2
#define KRG_ICON_GIT "\xef\x87\x93"	// U+f1d3
#define KRG_ICON_HACKER_NEWS "\xef\x87\x94"	// U+f1d4
#define KRG_ICON_TENCENT_WEIBO "\xef\x87\x95"	// U+f1d5
#define KRG_ICON_QQ "\xef\x87\x96"	// U+f1d6
#define KRG_ICON_WEIXIN "\xef\x87\x97"	// U+f1d7
#define KRG_ICON_PAPER_PLANE "\xef\x87\x98"	// U+f1d8
#define KRG_ICON_PAPER_PLANE_O "\xef\x87\x99"	// U+f1d9
#define KRG_ICON_HISTORY "\xef\x87\x9a"	// U+f1da
#define KRG_ICON_CIRCLE_THIN "\xef\x87\x9b"	// U+f1db
#define KRG_ICON_HEADER "\xef\x87\x9c"	// U+f1dc
#define KRG_ICON_PARAGRAPH "\xef\x87\x9d"	// U+f1dd
#define KRG_ICON_SLIDERS "\xef\x87\x9e"	// U+f1de
#define KRG_ICON_SHARE_ALT "\xef\x87\xa0"	// U+f1e0
#define KRG_ICON_SHARE_ALT_SQUARE "\xef\x87\xa1"	// U+f1e1
#define KRG_ICON_BOMB "\xef\x87\xa2"	// U+f1e2
#define KRG_ICON_FUTBOL_O "\xef\x87\xa3"	// U+f1e3
#define KRG_ICON_TTY "\xef\x87\xa4"	// U+f1e4
#define KRG_ICON_BINOCULARS "\xef\x87\xa5"	// U+f1e5
#define KRG_ICON_PLUG "\xef\x87\xa6"	// U+f1e6
#define KRG_ICON_SLIDESHARE "\xef\x87\xa7"	// U+f1e7
#define KRG_ICON_TWITCH "\xef\x87\xa8"	// U+f1e8
#define KRG_ICON_YELP "\xef\x87\xa9"	// U+f1e9
#define KRG_ICON_NEWSPAPER_O "\xef\x87\xaa"	// U+f1ea
#define KRG_ICON_WIFI "\xef\x87\xab"	// U+f1eb
#define KRG_ICON_CALCULATOR "\xef\x87\xac"	// U+f1ec
#define KRG_ICON_PAYPAL "\xef\x87\xad"	// U+f1ed
#define KRG_ICON_GOOGLE_WALLET "\xef\x87\xae"	// U+f1ee
#define KRG_ICON_CC_VISA "\xef\x87\xb0"	// U+f1f0
#define KRG_ICON_CC_MASTERCARD "\xef\x87\xb1"	// U+f1f1
#define KRG_ICON_CC_DISCOVER "\xef\x87\xb2"	// U+f1f2
#define KRG_ICON_CC_AMEX "\xef\x87\xb3"	// U+f1f3
#define KRG_ICON_CC_PAYPAL "\xef\x87\xb4"	// U+f1f4
#define KRG_ICON_CC_STRIPE "\xef\x87\xb5"	// U+f1f5
#define KRG_ICON_BELL_SLASH "\xef\x87\xb6"	// U+f1f6
#define KRG_ICON_BELL_SLASH_O "\xef\x87\xb7"	// U+f1f7
#define KRG_ICON_TRASH "\xef\x87\xb8"	// U+f1f8
#define KRG_ICON_COPYRIGHT "\xef\x87\xb9"	// U+f1f9
#define KRG_ICON_AT "\xef\x87\xba"	// U+f1fa
#define KRG_ICON_EYEDROPPER "\xef\x87\xbb"	// U+f1fb
#define KRG_ICON_PAINT_BRUSH "\xef\x87\xbc"	// U+f1fc
#define KRG_ICON_BIRTHDAY_CAKE "\xef\x87\xbd"	// U+f1fd
#define KRG_ICON_AREA_CHART "\xef\x87\xbe"	// U+f1fe
#define KRG_ICON_PIE_CHART "\xef\x88\x80"	// U+f200
#define KRG_ICON_LINE_CHART "\xef\x88\x81"	// U+f201
#define KRG_ICON_LASTFM "\xef\x88\x82"	// U+f202
#define KRG_ICON_LASTFM_SQUARE "\xef\x88\x83"	// U+f203
#define KRG_ICON_TOGGLE_OFF "\xef\x88\x84"	// U+f204
#define KRG_ICON_TOGGLE_ON "\xef\x88\x85"	// U+f205
#define KRG_ICON_BICYCLE "\xef\x88\x86"	// U+f206
#define KRG_ICON_BUS "\xef\x88\x87"	// U+f207
#define KRG_ICON_IOXHOST "\xef\x88\x88"	// U+f208
#define KRG_ICON_ANGELLIST "\xef\x88\x89"	// U+f209
#define KRG_ICON_CC "\xef\x88\x8a"	// U+f20a
#define KRG_ICON_ILS "\xef\x88\x8b"	// U+f20b
#define KRG_ICON_MEANPATH "\xef\x88\x8c"	// U+f20c
#define KRG_ICON_BUYSELLADS "\xef\x88\x8d"	// U+f20d
#define KRG_ICON_CONNECTDEVELOP "\xef\x88\x8e"	// U+f20e
#define KRG_ICON_DASHCUBE "\xef\x88\x90"	// U+f210
#define KRG_ICON_FORUMBEE "\xef\x88\x91"	// U+f211
#define KRG_ICON_LEANPUB "\xef\x88\x92"	// U+f212
#define KRG_ICON_SELLSY "\xef\x88\x93"	// U+f213
#define KRG_ICON_SHIRTSINBULK "\xef\x88\x94"	// U+f214
#define KRG_ICON_SIMPLYBUILT "\xef\x88\x95"	// U+f215
#define KRG_ICON_SKYATLAS "\xef\x88\x96"	// U+f216
#define KRG_ICON_CART_PLUS "\xef\x88\x97"	// U+f217
#define KRG_ICON_CART_ARROW_DOWN "\xef\x88\x98"	// U+f218
#define KRG_ICON_DIAMOND "\xef\x88\x99"	// U+f219
#define KRG_ICON_SHIP "\xef\x88\x9a"	// U+f21a
#define KRG_ICON_USER_SECRET "\xef\x88\x9b"	// U+f21b
#define KRG_ICON_MOTORCYCLE "\xef\x88\x9c"	// U+f21c
#define KRG_ICON_STREET_VIEW "\xef\x88\x9d"	// U+f21d
#define KRG_ICON_HEARTBEAT "\xef\x88\x9e"	// U+f21e
#define KRG_ICON_VENUS "\xef\x88\xa1"	// U+f221
#define KRG_ICON_MARS "\xef\x88\xa2"	// U+f222
#define KRG_ICON_MERCURY "\xef\x88\xa3"	// U+f223
#define KRG_ICON_TRANSGENDER "\xef\x88\xa4"	// U+f224
#define KRG_ICON_TRANSGENDER_ALT "\xef\x88\xa5"	// U+f225
#define KRG_ICON_VENUS_DOUBLE "\xef\x88\xa6"	// U+f226
#define KRG_ICON_MARS_DOUBLE "\xef\x88\xa7"	// U+f227
#define KRG_ICON_VENUS_MARS "\xef\x88\xa8"	// U+f228
#define KRG_ICON_MARS_STROKE "\xef\x88\xa9"	// U+f229
#define KRG_ICON_MARS_STROKE_V "\xef\x88\xaa"	// U+f22a
#define KRG_ICON_MARS_STROKE_H "\xef\x88\xab"	// U+f22b
#define KRG_ICON_NEUTER "\xef\x88\xac"	// U+f22c
#define KRG_ICON_GENDERLESS "\xef\x88\xad"	// U+f22d
#define KRG_ICON_FACEBOOK_OFFICIAL "\xef\x88\xb0"	// U+f230
#define KRG_ICON_PINTEREST_P "\xef\x88\xb1"	// U+f231
#define KRG_ICON_WHATSAPP "\xef\x88\xb2"	// U+f232
#define KRG_ICON_SERVER "\xef\x88\xb3"	// U+f233
#define KRG_ICON_USER_PLUS "\xef\x88\xb4"	// U+f234
#define KRG_ICON_USER_TIMES "\xef\x88\xb5"	// U+f235
#define KRG_ICON_BED "\xef\x88\xb6"	// U+f236
#define KRG_ICON_VIACOIN "\xef\x88\xb7"	// U+f237
#define KRG_ICON_TRAIN "\xef\x88\xb8"	// U+f238
#define KRG_ICON_SUBWAY "\xef\x88\xb9"	// U+f239
#define KRG_ICON_MEDIUM "\xef\x88\xba"	// U+f23a
#define KRG_ICON_Y_COMBINATOR "\xef\x88\xbb"	// U+f23b
#define KRG_ICON_OPTIN_MONSTER "\xef\x88\xbc"	// U+f23c
#define KRG_ICON_OPENCART "\xef\x88\xbd"	// U+f23d
#define KRG_ICON_EXPEDITEDSSL "\xef\x88\xbe"	// U+f23e
#define KRG_ICON_BATTERY_FULL "\xef\x89\x80"	// U+f240
#define KRG_ICON_BATTERY_THREE_QUARTERS "\xef\x89\x81"	// U+f241
#define KRG_ICON_BATTERY_HALF "\xef\x89\x82"	// U+f242
#define KRG_ICON_BATTERY_QUARTER "\xef\x89\x83"	// U+f243
#define KRG_ICON_BATTERY_EMPTY "\xef\x89\x84"	// U+f244
#define KRG_ICON_MOUSE_POINTER "\xef\x89\x85"	// U+f245
#define KRG_ICON_I_CURSOR "\xef\x89\x86"	// U+f246
#define KRG_ICON_OBJECT_GROUP "\xef\x89\x87"	// U+f247
#define KRG_ICON_OBJECT_UNGROUP "\xef\x89\x88"	// U+f248
#define KRG_ICON_STICKY_NOTE "\xef\x89\x89"	// U+f249
#define KRG_ICON_STICKY_NOTE_O "\xef\x89\x8a"	// U+f24a
#define KRG_ICON_CC_JCB "\xef\x89\x8b"	// U+f24b
#define KRG_ICON_CC_DINERS_CLUB "\xef\x89\x8c"	// U+f24c
#define KRG_ICON_CLONE "\xef\x89\x8d"	// U+f24d
#define KRG_ICON_BALANCE_SCALE "\xef\x89\x8e"	// U+f24e
#define KRG_ICON_HOURGLASS_O "\xef\x89\x90"	// U+f250
#define KRG_ICON_HOURGLASS_START "\xef\x89\x91"	// U+f251
#define KRG_ICON_HOURGLASS_HALF "\xef\x89\x92"	// U+f252
#define KRG_ICON_HOURGLASS_END "\xef\x89\x93"	// U+f253
#define KRG_ICON_HOURGLASS "\xef\x89\x94"	// U+f254
#define KRG_ICON_HAND_ROCK_O "\xef\x89\x95"	// U+f255
#define KRG_ICON_HAND_PAPER_O "\xef\x89\x96"	// U+f256
#define KRG_ICON_HAND_SCISSORS_O "\xef\x89\x97"	// U+f257
#define KRG_ICON_HAND_LIZARD_O "\xef\x89\x98"	// U+f258
#define KRG_ICON_HAND_SPOCK_O "\xef\x89\x99"	// U+f259
#define KRG_ICON_HAND_POINTER_O "\xef\x89\x9a"	// U+f25a
#define KRG_ICON_HAND_PEACE_O "\xef\x89\x9b"	// U+f25b
#define KRG_ICON_TRADEMARK "\xef\x89\x9c"	// U+f25c
#define KRG_ICON_REGISTERED "\xef\x89\x9d"	// U+f25d
#define KRG_ICON_CREATIVE_COMMONS "\xef\x89\x9e"	// U+f25e
#define KRG_ICON_GG "\xef\x89\xa0"	// U+f260
#define KRG_ICON_GG_CIRCLE "\xef\x89\xa1"	// U+f261
#define KRG_ICON_TRIPADVISOR "\xef\x89\xa2"	// U+f262
#define KRG_ICON_ODNOKLASSNIKI "\xef\x89\xa3"	// U+f263
#define KRG_ICON_ODNOKLASSNIKI_SQUARE "\xef\x89\xa4"	// U+f264
#define KRG_ICON_GET_POCKET "\xef\x89\xa5"	// U+f265
#define KRG_ICON_WIKIPEDIA_W "\xef\x89\xa6"	// U+f266
#define KRG_ICON_SAFARI "\xef\x89\xa7"	// U+f267
#define KRG_ICON_CHROME "\xef\x89\xa8"	// U+f268
#define KRG_ICON_FIREFOX "\xef\x89\xa9"	// U+f269
#define KRG_ICON_OPERA "\xef\x89\xaa"	// U+f26a
#define KRG_ICON_INTERNET_EXPLORER "\xef\x89\xab"	// U+f26b
#define KRG_ICON_TELEVISION "\xef\x89\xac"	// U+f26c
#define KRG_ICON_CONTAO "\xef\x89\xad"	// U+f26d
#define KRG_ICON_500PX "\xef\x89\xae"	// U+f26e
#define KRG_ICON_AMAZON "\xef\x89\xb0"	// U+f270
#define KRG_ICON_CALENDAR_PLUS_O "\xef\x89\xb1"	// U+f271
#define KRG_ICON_CALENDAR_MINUS_O "\xef\x89\xb2"	// U+f272
#define KRG_ICON_CALENDAR_TIMES_O "\xef\x89\xb3"	// U+f273
#define KRG_ICON_CALENDAR_CHECK_O "\xef\x89\xb4"	// U+f274
#define KRG_ICON_INDUSTRY "\xef\x89\xb5"	// U+f275
#define KRG_ICON_MAP_PIN "\xef\x89\xb6"	// U+f276
#define KRG_ICON_MAP_SIGNS "\xef\x89\xb7"	// U+f277
#define KRG_ICON_MAP_O "\xef\x89\xb8"	// U+f278
#define KRG_ICON_MAP "\xef\x89\xb9"	// U+f279
#define KRG_ICON_COMMENTING "\xef\x89\xba"	// U+f27a
#define KRG_ICON_COMMENTING_O "\xef\x89\xbb"	// U+f27b
#define KRG_ICON_HOUZZ "\xef\x89\xbc"	// U+f27c
#define KRG_ICON_VIMEO "\xef\x89\xbd"	// U+f27d
#define KRG_ICON_BLACK_TIE "\xef\x89\xbe"	// U+f27e
#define KRG_ICON_FONTICONS "\xef\x8a\x80"	// U+f280
#define KRG_ICON_REDDIT_ALIEN "\xef\x8a\x81"	// U+f281
#define KRG_ICON_EDGE "\xef\x8a\x82"	// U+f282
#define KRG_ICON_CREDIT_CARD_ALT "\xef\x8a\x83"	// U+f283
#define KRG_ICON_CODIEPIE "\xef\x8a\x84"	// U+f284
#define KRG_ICON_MODX "\xef\x8a\x85"	// U+f285
#define KRG_ICON_FORT_AWESOME "\xef\x8a\x86"	// U+f286
#define KRG_ICON_USB "\xef\x8a\x87"	// U+f287
#define KRG_ICON_PRODUCT_HUNT "\xef\x8a\x88"	// U+f288
#define KRG_ICON_MIXCLOUD "\xef\x8a\x89"	// U+f289
#define KRG_ICON_SCRIBD "\xef\x8a\x8a"	// U+f28a
#define KRG_ICON_PAUSE_CIRCLE "\xef\x8a\x8b"	// U+f28b
#define KRG_ICON_PAUSE_CIRCLE_O "\xef\x8a\x8c"	// U+f28c
#define KRG_ICON_STOP_CIRCLE "\xef\x8a\x8d"	// U+f28d
#define KRG_ICON_STOP_CIRCLE_O "\xef\x8a\x8e"	// U+f28e
#define KRG_ICON_SHOPPING_BAG "\xef\x8a\x90"	// U+f290
#define KRG_ICON_SHOPPING_BASKET "\xef\x8a\x91"	// U+f291
#define KRG_ICON_HASHTAG "\xef\x8a\x92"	// U+f292
#define KRG_ICON_BLUETOOTH "\xef\x8a\x93"	// U+f293
#define KRG_ICON_BLUETOOTH_B "\xef\x8a\x94"	// U+f294
#define KRG_ICON_PERCENT "\xef\x8a\x95"	// U+f295
#define KRG_ICON_GITLAB "\xef\x8a\x96"	// U+f296
#define KRG_ICON_WPBEGINNER "\xef\x8a\x97"	// U+f297
#define KRG_ICON_WPFORMS "\xef\x8a\x98"	// U+f298
#define KRG_ICON_ENVIRA "\xef\x8a\x99"	// U+f299
#define KRG_ICON_UNIVERSAL_ACCESS "\xef\x8a\x9a"	// U+f29a
#define KRG_ICON_WHEELCHAIR_ALT "\xef\x8a\x9b"	// U+f29b
#define KRG_ICON_QUESTION_CIRCLE_O "\xef\x8a\x9c"	// U+f29c
#define KRG_ICON_BLIND "\xef\x8a\x9d"	// U+f29d
#define KRG_ICON_AUDIO_DESCRIPTION "\xef\x8a\x9e"	// U+f29e
#define KRG_ICON_VOLUME_CONTROL_PHONE "\xef\x8a\xa0"	// U+f2a0
#define KRG_ICON_BRAILLE "\xef\x8a\xa1"	// U+f2a1
#define KRG_ICON_ASSISTIVE_LISTENING_SYSTEMS "\xef\x8a\xa2"	// U+f2a2
#define KRG_ICON_AMERICAN_SIGN_LANGUAGE_INTERPRETING "\xef\x8a\xa3"	// U+f2a3
#define KRG_ICON_DEAF "\xef\x8a\xa4"	// U+f2a4
#define KRG_ICON_GLIDE "\xef\x8a\xa5"	// U+f2a5
#define KRG_ICON_GLIDE_G "\xef\x8a\xa6"	// U+f2a6
#define KRG_ICON_SIGN_LANGUAGE "\xef\x8a\xa7"	// U+f2a7
#define KRG_ICON_LOW_VISION "\xef\x8a\xa8"	// U+f2a8
#define KRG_ICON_VIADEO "\xef\x8a\xa9"	// U+f2a9
#define KRG_ICON_VIADEO_SQUARE "\xef\x8a\xaa"	// U+f2aa
#define KRG_ICON_SNAPCHAT "\xef\x8a\xab"	// U+f2ab
#define KRG_ICON_SNAPCHAT_GHOST "\xef\x8a\xac"	// U+f2ac
#define KRG_ICON_SNAPCHAT_SQUARE "\xef\x8a\xad"	// U+f2ad
#define KRG_ICON_PIED_PIPER "\xef\x8a\xae"	// U+f2ae
#define KRG_ICON_FIRST_ORDER "\xef\x8a\xb0"	// U+f2b0
#define KRG_ICON_YOAST "\xef\x8a\xb1"	// U+f2b1
#define KRG_ICON_THEMEISLE "\xef\x8a\xb2"	// U+f2b2
#define KRG_ICON_GOOGLE_PLUS_OFFICIAL "\xef\x8a\xb3"	// U+f2b3
#define KRG_ICON_FONT_AWESOME "\xef\x8a\xb4"	// U+f2b4
#define KRG_ICON_HANDSHAKE_O "\xef\x8a\xb5"	// U+f2b5
#define KRG_ICON_ENVELOPE_OPEN "\xef\x8a\xb6"	// U+f2b6
#define KRG_ICON_ENVELOPE_OPEN_O "\xef\x8a\xb7"	// U+f2b7
#define KRG_ICON_LINODE "\xef\x8a\xb8"	// U+f2b8
#define KRG_ICON_ADDRESS_BOOK "\xef\x8a\xb9"	// U+f2b9
#define KRG_ICON_ADDRESS_BOOK_O "\xef\x8a\xba"	// U+f2ba
#define KRG_ICON_ADDRESS_CARD "\xef\x8a\xbb"	// U+f2bb
#define KRG_ICON_ADDRESS_CARD_O "\xef\x8a\xbc"	// U+f2bc
#define KRG_ICON_USER_CIRCLE "\xef\x8a\xbd"	// U+f2bd
#define KRG_ICON_USER_CIRCLE_O "\xef\x8a\xbe"	// U+f2be
#define KRG_ICON_USER_O "\xef\x8b\x80"	// U+f2c0
#define KRG_ICON_ID_BADGE "\xef\x8b\x81"	// U+f2c1
#define KRG_ICON_ID_CARD "\xef\x8b\x82"	// U+f2c2
#define KRG_ICON_ID_CARD_O "\xef\x8b\x83"	// U+f2c3
#define KRG_ICON_QUORA "\xef\x8b\x84"	// U+f2c4
#define KRG_ICON_FREE_CODE_CAMP "\xef\x8b\x85"	// U+f2c5
#define KRG_ICON_TELEGRAM "\xef\x8b\x86"	// U+f2c6
#define KRG_ICON_THERMOMETER_FULL "\xef\x8b\x87"	// U+f2c7
#define KRG_ICON_THERMOMETER_THREE_QUARTERS "\xef\x8b\x88"	// U+f2c8
#define KRG_ICON_THERMOMETER_HALF "\xef\x8b\x89"	// U+f2c9
#define KRG_ICON_THERMOMETER_QUARTER "\xef\x8b\x8a"	// U+f2ca
#define KRG_ICON_THERMOMETER_EMPTY "\xef\x8b\x8b"	// U+f2cb
#define KRG_ICON_SHOWER "\xef\x8b\x8c"	// U+f2cc
#define KRG_ICON_BATH "\xef\x8b\x8d"	// U+f2cd
#define KRG_ICON_PODCAST "\xef\x8b\x8e"	// U+f2ce
#define KRG_ICON_WINDOW_MAXIMIZE "\xef\x8b\x90"	// U+f2d0
#define KRG_ICON_WINDOW_MINIMIZE "\xef\x8b\x91"	// U+f2d1
#define KRG_ICON_WINDOW_RESTORE "\xef\x8b\x92"	// U+f2d2
#define KRG_ICON_WINDOW_CLOSE "\xef\x8b\x93"	// U+f2d3
#define KRG_ICON_WINDOW_CLOSE_O "\xef\x8b\x94"	// U+f2d4
#define KRG_ICON_BANDCAMP "\xef\x8b\x95"	// U+f2d5
#define KRG_ICON_GRAV "\xef\x8b\x96"	// U+f2d6
#define KRG_ICON_ETSY "\xef\x8b\x97"	// U+f2d7
#define KRG_ICON_IMDB "\xef\x8b\x98"	// U+f2d8
#define KRG_ICON_RAVELRY "\xef\x8b\x99"	// U+f2d9
#define KRG_ICON_EERCAST "\xef\x8b\x9a"	// U+f2da
#define KRG_ICON_MICROCHIP "\xef\x8b\x9b"	// U+f2db
#define KRG_ICON_SNOWFLAKE_O "\xef\x8b\x9c"	// U+f2dc
#define KRG_ICON_SUPERPOWERS "\xef\x8b\x9d"	// U+f2dd
#define KRG_ICON_WPEXPLORER "\xef\x8b\x9e"	// U+f2de
#define KRG_ICON_MEETUP "\xef\x8b\xa0"	// U+f2e0
