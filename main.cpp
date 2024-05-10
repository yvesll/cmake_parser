
#include <iostream>
#include <sys/stat.h>
#include "cmParser.h"

int dumpYaml(std::vector<cmListFileFunction> functions) {
    std::ostringstream content;
    const std::string tab = "  ";

    for (auto const& impl : functions) {
        content << "- original_name: " <<  impl.OriginalName() << std::endl;
        content << tab << "line: " << impl.Line() << std::endl;
        content << tab << "line_end: " << impl.LineEnd() << std::endl;
        if (impl.Arguments().size() == 0) {
            continue;
        }
        content << tab << "args:" << std::endl;
        for (auto const& argument : impl.Arguments()) {
            content << tab << "- value: " << argument.Value << std::endl;
            content << tab << tab << "line: " << argument.Line << std::endl;
        }
    }

    std::cout << content.str();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "You must provide  one cmakelist or cmake file." << std::endl;
        return -1;
    }

    struct stat info;

    if( stat( argv[1], &info ) != 0 ) {
        std::cerr << "Not a valid path" << std::endl;
        return -1;
    }
    cmListFileParser parser;
    parser.ParseFile(argv[1]);
    dumpYaml(parser.Functions);

    return 0;
}