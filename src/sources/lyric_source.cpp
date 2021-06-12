#include "stdafx.h"

#include "logging.h"
#include "lyric_source.h"

static std::vector<LyricSourceBase*> g_lyric_sources;

LyricSourceBase* LyricSourceBase::get(GUID id)
{
    for(LyricSourceBase* src : g_lyric_sources)
    {
        if(src->id() == id)
        {
            return src;
        }
    }

    return nullptr;
}

std::vector<GUID> LyricSourceBase::get_all_ids()
{
    std::vector<GUID> result;
    result.reserve(g_lyric_sources.size());
    for(LyricSourceBase* src : g_lyric_sources)
    {
        result.push_back(src->id());
    }
    return result;
}

void LyricSourceBase::on_init()
{
    g_lyric_sources.push_back(this);
}

std::string_view LyricSourceBase::get_metadata(metadb_handle_ptr track, const char* tag)
{
    const metadb_info_container::ptr& track_info_container = track->get_info_ref();
    const file_info& track_info = track_info_container->info();

    size_t meta_index = track_info.meta_find(tag);
    if((meta_index != pfc::infinite_size) && (track_info.meta_enum_value_count(meta_index) > 0))
    {
        std::string_view result = track_info.meta_enum_value(meta_index, 0);
        return trim_surrounding_whitespace(result);
    }

    return "";
}

std::string_view LyricSourceBase::get_artist(metadb_handle_ptr track)
{
    return get_metadata(track, "artist");
}

std::string_view LyricSourceBase::get_album(metadb_handle_ptr track)
{
    return get_metadata(track, "album");
}

std::string_view LyricSourceBase::get_title(metadb_handle_ptr track)
{
    std::string_view result = get_metadata(track, "title");
    if(preferences::searching::exclude_trailing_brackets())
    {
        result = trim_surrounding_whitespace(trim_trailing_text_in_brackets(result));
    }
    return result;
}

std::string_view LyricSourceBase::get_tracknumber(metadb_handle_ptr track)
{
    return get_metadata(track, "tracknumber");
}

std::string_view LyricSourceBase::trim_surrounding_whitespace(std::string_view str)
{
    size_t first_non_whitespace = str.find_first_not_of("\r\n ");
    size_t last_non_whitespace = str.find_last_not_of("\r\n ");

    if(first_non_whitespace == std::string_view::npos)
    {
        return "";
    }
    size_t len = (last_non_whitespace+1) - first_non_whitespace;
    return str.substr(first_non_whitespace, len);
}

std::string_view LyricSourceBase::trim_trailing_text_in_brackets(std::string_view str)
{
    std::string_view result = str;
    while(true)
    {
        size_t open_index = result.find_last_of("([{");
        if(open_index == std::string_view::npos)
        {
            break; // Nothing to trim
        }

        if(open_index == 0)
        {
            break; // Don't trim the entire string!
        }

        char opener = result[open_index];
        char closer = '\0';
        switch(opener)
        {
            case '[': closer = ']'; break;
            case '(': closer = ')'; break;
            case '{': closer = '}'; break;
        }
        assert(closer != '\0');

        size_t close_index = result.find_first_of(closer, open_index);
        if(close_index == std::string_view::npos)
        {
            break; // Unmatched open-bracket
        }

        result = result.substr(0, open_index);
    }

    return result;
}

bool LyricSourceRemote::is_local() const
{
    return false;
}

std::string LyricSourceRemote::save(metadb_handle_ptr /*track*/, bool /*is_timestamped*/, std::string_view /*lyrics*/, bool /*allow_ovewrite*/, abort_callback& /*abort*/)
{
    LOG_WARN("Cannot save lyrics to a remote source");
    assert(false);
    return "";
}
