#ifndef WASP_TOKENPOOL_I_H
#define WASP_TOKENPOOL_I_H

// default constructor
template<typename TTS, typename TITS, typename FOTS>
TokenPool<TTS, TITS, FOTS>::TokenPool()
{
}
// copy constructor
template<typename TTS, typename TITS, typename FOTS>
TokenPool<TTS, TITS, FOTS>::TokenPool(const TokenPool<TTS, TITS, FOTS>& orig)
    : m_strings(orig.m_strings)
    , m_tokens(orig.m_tokens)
    , m_line_offsets(orig.m_line_offsets)
{
}
// default destructor
template<typename TTS, typename TITS, typename FOTS>
TokenPool<TTS, TITS, FOTS>::~TokenPool()
{
}
// GET THE TOKEN'S STRING DATA
template<typename TTS, typename TITS, typename FOTS>
const char* TokenPool<TTS, TITS, FOTS>::str(TITS index) const
{
    return m_strings.data(index);
}
// GET THE TOKEN'S LINE
template<typename TTS, typename TITS, typename FOTS>
std::size_t TokenPool<TTS, TITS, FOTS>::line(TITS index) const
{
    FOTS token_file_offset = m_tokens[index].m_token_file_offset;
    typename std::vector<FOTS>::const_iterator ub = std::upper_bound(
        m_line_offsets.begin(), m_line_offsets.end(), token_file_offset);

    auto line = std::distance(m_line_offsets.begin(), ub) + 1;
    return static_cast<std::size_t>(line);
}
// GET THE TOKEN'S COLUMN
template<typename TTS, typename TITS, typename FOTS>
std::size_t TokenPool<TTS, TITS, FOTS>::column(TITS index) const
{
    FOTS token_file_offset = m_tokens[index].m_token_file_offset;
    typename std::vector<FOTS>::const_iterator ub = std::upper_bound(
        m_line_offsets.begin(), m_line_offsets.end(), token_file_offset);
    std::size_t column = 0;
    // check if token is on first line
    if (ub == m_line_offsets.begin())
    {
        // make base 1
        column = token_file_offset + 1;
    }
    else
    {
        ub--;  // back it up 1 as the token's column is past the token's line
        // column is the token_offset - token_newline_offset
        column = token_file_offset - (*ub);
    }

    return column;
}

// GET THE TOKEN'S LAST LINE
template<typename TTS, typename TITS, typename FOTS>
std::size_t TokenPool<TTS, TITS, FOTS>::last_line(TITS index) const
{
    // token start offset, end position
    auto token_file_offset = m_tokens[index].m_token_file_offset;
    auto token_end_position =
        token_file_offset + std::strlen(m_strings.data(index)) - 1;

    // first newline after the end of the token
    auto ub = std::upper_bound(m_line_offsets.begin(), m_line_offsets.end(),
                               token_end_position);

    // calculate/return line number
    auto line = std::distance(m_line_offsets.begin(), ub) + 1;
    return static_cast<std::size_t>(line);
}

// GET THE TOKEN'S LAST_COLUMN
template<typename TTS, typename TITS, typename FOTS>
std::size_t TokenPool<TTS, TITS, FOTS>::last_column(TITS index) const
{
    // token start offset, end position
    auto token_file_offset = m_tokens[index].m_token_file_offset;
    auto token_end_position =
        token_file_offset + std::strlen(m_strings.data(index)) - 1;

    // first newline after the end of the token
    auto ub = std::upper_bound(m_line_offsets.begin(), m_line_offsets.end(),
                               token_end_position);

    // last column
    std::size_t column;

    // check if token is on first line
    if (ub == m_line_offsets.begin())
    {
        column = token_end_position + 1;
    }
    else
    {
        // back it up 1 as the token's column is past the token's line
        ub--;
        // column is the token_offset - token_newline_offset
        column = token_end_position - *ub;
    }

    return column;
}

// GET A TOKEN's OFFSET
template<typename TTS, typename TITS, typename FOTS>
FOTS TokenPool<TTS, TITS, FOTS>::offset(TITS token_index) const
{
    return m_tokens[token_index].m_token_file_offset;
}
// PUSH A NEW TOKEN
template<typename TTS, typename TITS, typename FOTS>
void TokenPool<TTS, TITS, FOTS>::push(const char* str,
                                      TTS  type,
                                      FOTS token_file_offset,
                                      bool track_newline)
{
    // capture the token's string in the string pool
    m_strings.push(str);
    m_tokens.push_back(Token(type, token_file_offset));

    // push embedded newlines
    for (size_t i = 0; str[i] != 0; i++)
    {
        // this is a newline, push its offset
        if (str[i] == '\n' && track_newline)
        {
            m_line_offsets.push_back(token_file_offset + i);
        }
    }
}
// PUSH A NEW LINE
template<typename TTS, typename TITS, typename FOTS>
void TokenPool<TTS, TITS, FOTS>::push_line(FOTS line_file_offset)
{
    m_line_offsets.push_back(line_file_offset);
}
// GET A TOKEN'S TYPE
template<typename TTS, typename TITS, typename FOTS>
TTS TokenPool<TTS, TITS, FOTS>::type(TITS index) const
{
    return m_tokens[index].m_token_type;
}
// GET A LINE's OFFSET
template<typename TTS, typename TITS, typename FOTS>
FOTS TokenPool<TTS, TITS, FOTS>::line_offset(TITS line_index) const
{
    return m_line_offsets[line_index];
}
#endif
