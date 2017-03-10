#include <yaml.h>

#include <stdlib.h>
#include <stdio.h>

#include <iostream>


int main(int argc, char *argv[]) {
    int number;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " file1.yaml ..." << std::endl;
        return 0;
    }

    for (number = 1; number < argc; number ++) {
        FILE *file;
        yaml_parser_t parser;
        yaml_event_t event;
        int done = 0;
        int count = 0;
        int error = 0;

        std::cout << "[" << number << "] Parsing '" << argv[number]
                  << "': " << std::endl;
        fflush(stdout);

        file = fopen(argv[number], "rb");

        yaml_parser_initialize(&parser);
        yaml_parser_set_input_file(&parser, file);

        while (!done) {
            if (!yaml_parser_parse(&parser, &event)) {
                error = 1;
                break;
            }



            done = (event.type == YAML_STREAM_END_EVENT);
            yaml_event_delete(&event);
            count ++;
        }

        yaml_parser_delete(&parser);

        fclose(file);
        std::cout << (error ? "FAILURE" : "SUCCESS") << " (" << count << " events)" << std::endl;
    }

    return 0;
}