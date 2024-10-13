
#include <format>

#include "diagnostics.hh"

std::string indent(size_t length, char div)
{
    std::string ind = "";
    for (size_t i = 0; i < length; i++)
        ind += div;

    return ind;
}

void Diagnostics::log(Message message, LogLevel level)
{
    if (this->lvl > level)
        return;
    
    // std::lock_guard<std::mutex> lock(mtx); TODO: this may be neccessary with multiple threads, now it is useless
    std::string level_name = level_names.find(level)->second;

    std::string color_code = (use_colors) ? color_codes.find(level)->second : "";
    std::string bold_color = (use_colors) ? WHITE : "";
    std::string color_rst = (use_colors) ? RESET : "";
    std::string output = std::format("{0}{1}:{2}:{3}: {4}{5}{6} {7}", bold_color,
                                                              message.file,
                                                              message.loc.row,
                                                              message.loc.col,
                                                              color_code,
                                                              level_name,
                                                              color_rst,
                                                              message.msg);

    std::string side_panel = std::format("  {0} | ", message.loc.row);
    std::string code_chunk = std::format("{0}{1}{2}{3}{4}", message.chunk.substr(0, message.loc.col - 1),
                                                            color_code,
                                                            message.chunk.substr(message.loc.col - 1, message.length),
                                                            color_rst,
                                                            message.chunk.substr(message.loc.col + message.length -1));


    if (dest.is_open())
    {
        // TODO: proper formatting is needed!
        dest << output << std::endl;
        dest << std::format("    {0} | {1}", message.loc.col, message.chunk) << std::endl;
        dest << std::format("      |  {0}\n", message.other_info) << std::endl;
    }
    else
    {
        std::cout << output << std::endl;
        std::cout << side_panel << code_chunk << std::endl;
        std::cout << indent(side_panel.length() - 2) << std::format("| {0}{1}^{2} |> {3}{4}",
                                                                                         indent(message.chunk.substr(0, message.loc.col).length() - 1),
                                                                                         color_code, 
                                                                                         (message.length > 1) ? indent(message.length - 1, '~') : indent(3, '-'),
                                                                                         message.other_info,
                                                                                         color_rst) << std::endl;
        std::cout << indent(side_panel.length() - 2) << "|" << std::endl;
    }
}

void Diagnostics::reportResult(bool verdict)
{
    fsec duration = Time::now() - begin_time;
    std::string output;

    if (use_colors)
    {
        output = (verdict) ?
            std::format(COMPILATION_OK, WHITE, (verdict ? GREEN : RED), WHITE, std::chrono::duration_cast<ms>(duration), RESET) :
            std::format(COMPILATION_ERROR, WHITE, (verdict ? GREEN : RED), WHITE, std::chrono::duration_cast<ms>(duration), RESET);
    }
    else
    {
        output = (verdict) ?
            std::format(COMPILATION_OK, "", "", "", std::chrono::duration_cast<ms>(duration), "") :
            std::format(COMPILATION_ERROR, "", "", "", std::chrono::duration_cast<ms>(duration), "");
    }

    if (dest.is_open())
    {
        dest << output << std::endl;
        dest << '\n';
    }
    else
    {
        std::cout << output << std::endl;
        std::cout << '\n';
    }

    if (!verdict)
        exit(2);
}
