#include "generator.h"
#include "parser.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main(const int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: lisp-compiler <filename>" << std::endl;
      return 1;
    }

    const std::string filename = argv[1];
    if (std::ifstream file(filename); !file.is_open()) {
      std::cerr << "Could not open file: " << filename << std::endl;
      return 1;
    }

    std::string source;
    std::ifstream file(filename);
    source.assign((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());

    parser::Parser parser(source);
    auto ast = parser.parse();
    // printAST(ast); // uncomment to print the AST

    generator::Generator generator(ast);
    const auto output = generator.generate();

    // write the generated code to a file at the same location as the input file
    std::filesystem::path input_path(filename);
    std::filesystem::path output_path = input_path.parent_path() / "middle.cpp";
    std::filesystem::path executable_path = input_path.parent_path() / "output";

    // Write the generated code to the output file
    std::ofstream out(output_path);
    if (!out) {
      throw std::ios_base::failure("Failed to open output file: " +
                                   output_path.string());
    }
    out << output;
    out.close();

    std::string compile_command =
        "g++ -O2 -o " + filename + ".out " + output_path.string();
    if (std::system(compile_command.c_str()) != 0) {
      throw std::runtime_error("Compilation failed");
    }

    std::cout << "Compilation successful. Executable created at: "
              << executable_path.string() << std::endl;

    const bool generate_assembly = true; // set to true to generate assembly
    if (generate_assembly) {
      std::filesystem::path assembly_path =
          input_path.parent_path() / "output.s";
      std::string assembly_command = "g++ -O2 -masm=att -S -o " +
                                     assembly_path.string() + " " +
                                     output_path.string();
      if (std::system(assembly_command.c_str()) != 0) {
        throw std::runtime_error("Assembly generation failed");
      }
      std::cout << "Assembly generated at: " << assembly_path.string()
                << std::endl;
    }
    std::filesystem::remove(output_path); // comment this line to keep the
    // generated code

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

