
#include <format>

#include "diagnostics.hh"

std::string indent(size_t length)
{
    std::string ind = "";
    for (size_t i = 0; i < length; i++)
        ind += " ";

    return ind;
}

void Diagnostics::log(Message message, LogLevel level)
{
    if (this->lvl > level)
        return;
    
    // std::lock_guard<std::mutex> lock(mtx); TODO: this may be neccessary with multiple threads, now it is useless
    std::string level_name = level_names.find(level)->second;

    std::string color_code = (use_colors) ? color_codes.find(level)->second : "";
    std::string color_rst = (use_colors) ? RESET : "";
    std::string output = std::format("{0}{1}:{2}:{3}: {4}{5}{6} {7}", WHITE, 
                                                              message.file, 
                                                              message.loc.col, 
                                                              message.loc.row, 
                                                              color_code, 
                                                              level_name, 
                                                              color_rst, 
                                                              message.msg);


    if (dest.is_open())
    {
        // TODO: proper formatting is needed!
        dest << output << std::endl;
        dest << std::format("    {0} | {1}", message.loc.col, message.other_info) << std::endl;
    }
    else
    {
        std::cout << output << std::endl;
        std::cout << std::format("    {0} | {1}", message.loc.col, message.other_info) << std::endl;
    }
}
