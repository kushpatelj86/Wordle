#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

// Credits and thanks goes to CPSC121 student developer Esteban Escartin.
int main() {
  char command_selection;

  if (system(nullptr) == 1) {
    // Clean up output files and compile the server.
    std::string command = "make build";
    // Run command made above
    int status = system(command.c_str());
    if (status != 0) {
      return 0;
    }
    while (true) {
      std::cout << std::endl
                << "Would you like to run make commands?\n"
                   "R: run the Tuffle Backend server\n"
                   "A: run all Tuffle unit tests\n"
                   "B: make test_boardcolors (tests boardColors)\n"
                   "D: make test_dictionary (tests Dictionary class)\n"
                   "E: make test_errormessage (tests errorMessage)\n"
                   "G: make test_gamestatus (tests gameStatus)\n"
                   "W: make test_guessedwords (tests guessedWords)\n"
                   "T: make test (general TuffleGame tests)\nS: make "
                   "stylecheck\nF: make formatcheck\n"
                   ""
                   "Press any other key to exit.\nSelection: "
                << std::endl;
      std::cin >> command_selection;
      bool exit = false;
      switch (std::tolower(command_selection)) {
        case 'r':
          command = "./main";
          system(command.c_str());
          break;
        case 'a':
          command =
              "make test_boardcolors && make test_dictionary && make "
              "test_errormessage && make test_gamestatus && make "
              "test_guessedwords && make test";
          system(command.c_str());
          break;
        case 'b':
          command = "make test_boardcolors";
          system(command.c_str());
          break;
        case 'd':
          command = "make test_dictionary";
          system(command.c_str());
          break;
        case 'e':
          command = "make test_errormessage";
          system(command.c_str());
          break;
        case 'g':
          command = "make test_gamestatus";
          system(command.c_str());
          break;
        case 'w':
          command = "make test_guessedwords";
          system(command.c_str());
          break;
        case 't':
          command = "make test";
          system(command.c_str());
          break;
        case 's':
          command = "make clean && make stylecheck";
          system(command.c_str());
          break;
        case 'f':
          command = "make clean && make formatcheck";
          system(command.c_str());
          break;
        default:
          exit = true;
          break;
      }
      if (exit) {
        break;
      }
    }
    std::cout << "Done!" << std::endl;
  }
}