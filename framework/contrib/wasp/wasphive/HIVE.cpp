/*
 * File:   HIVE.cpp
 * Author: Brandon Langley, Robert Lefebvre (migrated to WASP)
 *
 * Created on September 17, 2014, 8:19 PM
 */

#include "HIVE.h"

#define doj wasp
#include "wasphive/AlphaNum.h"  // special alpha numeric sort logic
#undef doj

using namespace std;

static const std::atomic<bool> GLOBAL_STOP(false);

namespace wasp
{
HIVE::HIVE() : stop(GLOBAL_STOP)
{
}

HIVE::HIVE(const std::atomic<bool>& stop) : stop(stop)
{
}

HIVE::~HIVE()
{
}
void HIVE::sort_errors(std::vector<string>& errors)
{
    std::sort(errors.begin(), errors.end(), alphanum_less<std::string>());
}

void HIVE::printMessages(bool             pass,
                         vector<string>&  errors,
                         MessagePrintType msgType,
                         string           file,
                         std::ostream&    output)
{
    if (msgType == MessagePrintType::NORMAL)
    {
        output << endl
               << (file != "" ? file : "N/A") << " - "
               << (pass ? "PASS" : "FAIL") << endl;
        output
            << "-------------------------------------------------------------"
            << endl;
        for (size_t i = 0; i < errors.size(); i++)
        {
            output << errors[i] << endl;
        }
    }

    else if (msgType == MessagePrintType::XML)
    {
        output << "  <file name=\"" << (file != "" ? file : "N/A") << "\"";
        output << " pass=\"" << (pass ? "true" : "false") << "\"";
        output << " errors=\"" << errors.size();
        output << (errors.size() == 0 ? "\"/>" : "\">") << endl;
        for (size_t i = 0; i < errors.size(); i++)
        {
            std::size_t foundline;
            std::size_t foundcolumn;
            std::size_t founddash;
            string      line;
            string      column;
            string      message;
            foundline   = errors[i].find("line:");
            foundcolumn = errors[i].find(" column:");
            founddash   = errors[i].find(" - ");
            if (foundline == string::npos || foundcolumn == string::npos ||
                founddash == string::npos || foundline != 0)
            {
                line    = "N/A";
                column  = "N/A";
                message = errors[i];
            }
            else
            {
                line = errors[i].substr(foundline + 5,
                                        foundcolumn - (foundline + 5));
                column = errors[i].substr(foundcolumn + 8,
                                          founddash - (foundcolumn + 8));
                message = errors[i].substr(founddash + 3);
            }
            output << "    <error id=\"" << i + 1 << "\">" << endl;
            output << "      <location line=\"" << line << "\"";
            output << " column=\"" << column << "\"/>" << endl;
            output << "      <message>" << message << "</message>" << endl;
            output << "    </error>" << endl;
            if (i + 1 == errors.size())
                output << "  </file>" << endl;
        }
    }

    else if (msgType == MessagePrintType::JSON)
    {
        output << "{" << endl;
        output << "  \"file\":\"" << (file != "" ? file : "N/A") << "\","
               << endl;
        output << "  \"pass\":\"" << (pass ? "true" : "false") << "\"," << endl;
        output << "  \"count\":\"" << errors.size() << "\"";
        if (errors.size() != 0)
            output << "," << endl;
        else
            output << endl;
        for (size_t i = 0; i < errors.size(); i++)
        {
            if (i == 0)
                output << "  \"errors\":[" << endl;
            std::size_t foundline;
            std::size_t foundcolumn;
            std::size_t founddash;
            string      line;
            string      column;
            string      message;
            foundline   = errors[i].find("line:");
            foundcolumn = errors[i].find(" column:");
            founddash   = errors[i].find(" - ");
            if (foundline == string::npos || foundcolumn == string::npos ||
                founddash == string::npos || foundline != 0)
            {
                line    = "N/A";
                column  = "N/A";
                message = errors[i];
            }
            else
            {
                line = errors[i].substr(foundline + 5,
                                        foundcolumn - (foundline + 5));
                column = errors[i].substr(foundcolumn + 8,
                                          founddash - (foundcolumn + 8));
                message = errors[i].substr(founddash + 3);
            }
            std::replace(message.begin(), message.end(), '"', '\'');
            output << "    {" << endl;
            output << "      \"line\":\"" << line << "\"," << std::endl;
            output << "      \"column\":\"" << column << "\"," << std::endl;
            output << "      \"message\":\"" << message << "\"" << std::endl;
            output << "    }";
            if (i + 1 != errors.size())
                output << "," << endl;
            else
                output << endl << "  ]" << endl;
        }
        output << "}" << endl;
    }
}

}  // namespace wasp
