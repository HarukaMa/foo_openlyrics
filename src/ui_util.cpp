#include "stdafx.h"

#include "lyric_data.h"
#include "tag_util.h"

static std::string get_friendly_string(std::string_view artist, std::string_view title)
{
    std::string result;
    const bool has_artist = !artist.empty();
    const bool has_title = !title.empty();
    if(has_artist || has_title)
    {
        if(has_artist)
        {
            result += artist;
        }
        if(has_artist && has_title)
        {
            result += " - ";
        }
        if(has_title)
        {
            result += title;
        }
    }
    return result;
}

std::string get_track_friendly_string(const metadb_v2_rec_t& track_info)
{
    return get_friendly_string(track_metadata(track_info, "artist"), track_metadata(track_info, "title"));
}
