#include "waspcore/utils.h"
#include <fstream>

namespace wasp
{

bool load_file(const std::string& path, std::stringstream& s)
{
    std::ifstream f(path);
    bool          first = true;
    while (!f.eof() && f.good())
    {
        std::string line;
        std::getline(f, line);
        if (!first)
        {
            s << std::endl;
        }
        s << line;
        first = false;
    }
    return f.eof() && !f.bad();
}

std::string strip_quotes(const std::string& src)
{
    std::string s = src;
    // remove single quotes
    if (s.size() >= 2 && s[0] == '\'' && s[s.size() - 1] == '\'')
    {
        s.erase(0, 1);
        s.erase(s.size() - 1, 1);
        return s;
    }
    // remove double quotes
    if (s.size() >= 2 && s[0] == '"' && s[s.size() - 1] == '"')
    {
        s.erase(0, 1);
        s.erase(s.size() - 1, 1);
    }
    return s;
}  // strip_quotes

std::vector<std::string> split( const std::string& delim,
                                const std::string& str )
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string w( str );
    while( ( pos = w.find( delim ) ) != std::string::npos )
    {
        tokens.push_back( w.substr( 0, pos ) );
        w.erase( 0, pos + delim.length() );
    }
    tokens.push_back( w );  // remainder

    // if last element is empty, delete it (only if it has more than 1)
    if( tokens.size() > 1 && tokens[tokens.size() - 1].length() == 0 )
    {
        tokens.resize( tokens.size() - 1 );
    }

    return tokens;
}  // split

std::string trim(std::string str, const std::string& char_seq)
{
    if (str.size() == char_seq.size() && str == char_seq)
        return "";
    // trim trailing char sequences
    size_t endpos = str.find_last_not_of(char_seq);
    if (std::string::npos != endpos)
    {
        str = str.substr(0, endpos + 1);
    }
    // trim leading char sequences
    size_t startpos = str.find_first_not_of(char_seq);
    if (std::string::npos != startpos)
    {
        str = str.substr(startpos);
    }
    return str;
}

std::string dir_name(const std::string& path)
{
    size_t sep_i = path.find_last_of("/\\");
    if (sep_i != std::string::npos)
        return path.substr(0, sep_i);
    return path;
}

bool file_exists(const std::string &path)
{
    std::ifstream relativeFile(path);
    return relativeFile.good();
}
std::string json_escape_string(const std::string& src)
{
    std::string dst;
    dst.reserve(src.size());
    for (char ch : src)
    {
        switch (ch)
        {
            case '\\':
                dst.append("\\\\");
                break;
            case '"':
                dst.append("\\\"");
                break;
            case '\b':
                dst.append("\\b");
                break;
            case '\f':
                dst.append("\\f");
                break;
            case '\n':
                dst.append("\\n");
                break;
            case '\r':
                dst.append("\\r");
                break;
            case '\t':
                dst.append("\\t");
                break;
            default:
                dst.push_back(ch);
                break;
        }
    }
    return dst;
}  // json_escape_string

std::string json_unescape_string(const std::string& src)
{
    std::string dst;
    dst.reserve(src.size());
    bool escape_state = false;
    for ( int i = 0 ; i < src.length() ; i++ )
    {
        char ch = src[i];
        if (!escape_state && ch == '\\')
        {
            if ( i + 5 < src.length() &&
                 src[i+1] == 'u'      &&
                 src[i+2] == '0'      &&
                 src[i+3] == '0'      &&
                 src[i+4] == '2'      &&
                 src[i+5] == '7'      )
            {
                dst.append("'");
                i+=5;
                continue;
            }
            if ( i + 5 < src.length() &&
                 src[i+1] == 'u'      &&
                 src[i+2] == '0'      &&
                 src[i+3] == '0'      &&
                 src[i+4] == '3'      &&
                 src[i+5] == 'c'      )
            {
                dst.append("<");
                i+=5;
                continue;
            }
            if ( i + 5 < src.length() &&
                 src[i+1] == 'u'      &&
                 src[i+2] == '0'      &&
                 src[i+3] == '0'      &&
                 src[i+4] == '3'      &&
                 src[i+5] == 'e'      )
            {
                dst.append(">");
                i+=5;
                continue;
            }
            escape_state = true;
        }
        else if (escape_state)
        {
            switch (ch)
            {
                case '\\':
                    dst.append("\\");
                    break;
                case '"' :
                    dst.append("\"");
                    break;
                case 'b' :
                    dst.append("\b");
                    break;
                case 'f' :
                    dst.append("\f");
                    break;
                case 'n' :
                    dst.append("\n");
                    break;
                case 'r' :
                    dst.append("\r");
                    break;
                case 't' :
                    dst.append("\t");
                    break;
                default:
                    dst.append("\\");
                    dst.push_back(ch);
                    break;
            }
            escape_state = false;
        }
        else
        {
            dst.push_back(ch);
        }
    }
    return dst;
}  // json_unescape_string

std::string xml_escape_data(const std::string& src)
{
    std::stringstream dst;
    for (char ch : src)
    {
        switch (ch)
        {
            case '&':
                dst << "&amp;";
                break;
            case '\'':
                dst << "&apos;";
                break;
            case '"':
                dst << "&quot;";
                break;
            case '<':
                dst << "&lt;";
                break;
            case '>':
                dst << "&gt;";
                break;
            default:
                dst << ch;
                break;
        }
    }
    return dst.str();
}  // xml_escape_data

std::string xml_escape_name(const std::string& src)
{
    std::stringstream dst;
    if (src.at(0) >= '0' && src.at(0) <= '9')
    {
        dst << "_";
    }
    for (char ch : src)
    {
        switch (ch)
        {
            case '{':
                dst << "LBC";
                break;
            case '}':
                dst << "RBC";
                break;
            case '[':
                dst << "LBK";
                break;
            case ']':
                dst << "RBK";
                break;
            case '(':
                dst << "LP";
                break;
            case ')':
                dst << "RP";
                break;
            case '-':
                dst << "MINUS";
                break;
            case '.':
                dst << "DOT";
                break;
            case '\'':
                dst << "APOS";
                break;
            case '=':
                dst << "ASSIGN";
                break;
            case ':':
                dst << "COLON";
                break;
            case ',':
                dst << "COMMA";
                break;
            case '/':
                dst << "FWDSLSH";
                break;
            case '\\':
                dst << "BCKSLSH";
                break;
            default:
                dst << ch;
                break;
        }
    }
    return dst.str();
}  // xml_escape_name
}
