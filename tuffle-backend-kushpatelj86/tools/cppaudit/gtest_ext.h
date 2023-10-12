// This file contains implementations that extend the functionality of the
// google test framework
// Version: 0.1.1-beta
 
#include <gtest/gtest.h>
#include <random>
#include <string>
#include <iomanip>
#include <future>
#include <chrono>
#include <map>
#include <fstream>
#include "termcolor/termcolor.hpp"
 
class SkipListener : public ::testing::EmptyTestEventListener
{
  private:
    size_t fatal_failures{0};
    // Fired before each individual test starts: before the test fixture is constructed
    // and SetUp() is invoked.
    void OnTestStart(const ::testing::TestInfo& info) override
    {
      if (fatal_failures > 0) {
         GTEST_SKIP() << "Test skipped until other errors are fixed";
      }
    }
    // Fired after the unit test runs.
    void OnTestPartResult(const ::testing::TestPartResult& result) override {
      if(result.failed()) {
        fatal_failures++;
      }
    }    
};
 
class UnitTestFileManager : public ::testing::Test {
public:
  UnitTestFileManager(const std::string &filename): filename_(filename) { }
protected:
  void SetUp() override {
    std::ifstream my_empty_file;
    my_empty_file.open(filename_);
    if (my_empty_file.good()) {
      rename(filename_.c_str(), (PREFIX + filename_).c_str());
      file_exists = true;
    }
    my_empty_file.close();
  }

  void TearDown() override {
    if (file_exists) {
      rename((PREFIX + filename_).c_str(), filename_.c_str());
    } else {
      remove(filename_.c_str());
    }
  }
  const std::string PREFIX{"u_test"};
  bool file_exists = false;
  std::string filename_;
};

// Run and retrieves the output of an executable program from
// the command line.
//
// @param prog_name name of the executable file
// @param input     keyboard input sent to the program
//
// @return output of the program
std::string exec_program(std::string prog_name, std::string input)
{
  FILE *fp = popen(("echo \""+ input +"\" | ./" + prog_name).c_str(), "r");
  char buf[1024];
  std::string output = "";
  while (fgets(buf, 1024, fp)) {
    output += buf;
  }
  pclose(fp);
  return output;
}
 
// Converts a double value to a formatted string
//
// @param val   value to be formatted
// @param prec  decimal precision
//
// @return string representation of the double value
std::string to_string_double(double val, const int prec = 2)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(prec) << val;
    return out.str();
}
 
// Generate a string with random values from an alphanumeric character set
//
// @param max_length  length of string to generate
//
// @return            randomly generated string
std::string generate_string(int max_length){
  std::string possible_characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  std::random_device rd;
  std::mt19937 engine(rd());
  std::uniform_int_distribution<> dist(0, possible_characters.size()-1);
  std::string ret = "";
  for(int i = 0; i < max_length; i++){
      int random_index = dist(engine); //get index between 0 and possible_characters.size()-1
      ret += possible_characters[random_index];
  }
  return ret;
}
 
std::string expose_special_characters(const std::string &source) {
  if(source.length() == 0) {
    return "<empty>";
  }
  std::map<char, std::string> replacement_map {
    {' ', "<space>"},
    {'\n', "\\n\n"}
  };
  std::stringstream exposed_string;
  for(int i = 0; i < source.length(); i++) {
    if(replacement_map.find(source[i]) != replacement_map.end()) {   
      exposed_string << replacement_map[source[i]];
    } else {
      exposed_string << source[i];
    }
  }
  return exposed_string.str();
}
 
// This macro is used to simulate the standard input (cin) for a code block
//
// @param input       simulated input
// @param statement   lambda code block to run
#define SIMULATE_CIN(input, statement) { \
  std::stringstream input_ss, output_ss; \
  auto old_output_buf = std::cout.rdbuf(output_ss.rdbuf()); \
  auto old_input_buf = std::cin.rdbuf(input_ss.rdbuf()); \
  input_ss.str(input); \
  statement; \
  std::cin.rdbuf(old_input_buf); \
  std::cout.rdbuf(old_output_buf); \
}
 
::testing::AssertionResult AssertExecStdOut(const char* prog_name_expr,
                                        const char* prog_input_expr,
                                        const char* prog_output_expr,
                                        std::string prog_name, 
                                        std::string prog_input,
                                        std::string prog_output) {
  if ( access( prog_name.c_str(), F_OK ) == -1 ) {
    return ::testing::AssertionFailure() << "      cannot test '" << prog_name 
                                         << "': Make sure your executable file"
                                         << " is called '" << prog_name << "'";
  }
  std::string exec_output = exec_program(prog_name, prog_input);
  if (exec_output == prog_output) {
    return ::testing::AssertionSuccess();
  } else {
    int pos = 0;
    int line_pos = 0;
    int char_pos = 0;
    std::string exec_diff = "";
    std::string prog_diff = "";
    bool found_diff = false;
    for(; pos < exec_output.length(); pos++) {
      if (exec_output[pos] == '\n') {
        line_pos++;
        char_pos = 0;
      }
      char_pos++;      
      exec_diff = exec_output[pos];
      if (pos >= prog_output.length()) {
        break;
      } else if(prog_output[pos] != exec_output[pos]) {
        prog_diff = prog_output[pos];
        found_diff = true;
        break;
      } 
    }
    if(!found_diff && pos < prog_output.length()) {
      exec_diff = "";
      prog_diff = prog_output[pos];
    }
    exec_diff = expose_special_characters(exec_diff);
    prog_diff = expose_special_characters(prog_diff);
 
    std::ostringstream error_str_stream;
    std::ostringstream exec_str_stream;
    exec_str_stream << termcolor::colorize << termcolor::green 
                    << exec_output.substr(0, pos)
                    << termcolor::red << exec_output.substr(pos)
                    << termcolor::reset;
 
    std::ostringstream prog_str_stream;
    prog_str_stream << termcolor::colorize << termcolor::green 
                    << prog_output.substr(0, pos)
                    << termcolor::red << prog_output.substr(pos)
                    << termcolor::reset;
 
    error_str_stream << "Your program's output did not match the expected "
                     << "output starting on line " << line_pos + 1 
                     << " character " << char_pos 
                     << ".\nExpected " << prog_diff
                     << " instead of " << exec_diff
                     //<< "\n\nExpected output: \n" << expose_special_characters(prog_str_stream.str()) 
                     << "\n\nExpected output: \n" << prog_str_stream.str()
                     << "\n\nYour program's output: \n"
                     //<< expose_special_characters(exec_str_stream.str()) << "\n\nTest Input: \n" 
                     << exec_str_stream.str() << "\n\nInput: \n"
                     << prog_input ;
     
    return ::testing::AssertionFailure() << error_str_stream.str();
  }
}
 
// This macro checks if the output of an executable program matches an expected
// output.
//
// @param prog_name name of the executable file
// @param input     keyboard input sent to the program
// @param output    expected output of the program
#define ASSERT_EXECEQ(prog_name, input, output) \
    EXPECT_PRED_FORMAT3(AssertExecStdOut, prog_name, input, output)
 
template <typename T>
::testing::AssertionResult AssertExecMatcher(const char* prog_name_expr,
                                        const char* prog_input_expr,
                                        const char* matcher_expr,
                                        std::string prog_name, 
                                        std::string prog_input,
                                        T matcher) {
  if ( access( prog_name.c_str(), F_OK ) == -1 ) {
    return ::testing::AssertionFailure() << "      cannot test '" << prog_name 
                                         << "': Make sure your executable file"
                                         << " is called '" << prog_name << "'";
  }
 
  std::string exec_output = exec_program(prog_name, prog_input);
  // based on https://github.com/google/googletest/blob/fb49e6c164490a227bbb7cf5223b846c836a0305/googlemock/include/gmock/gmock-matchers.h#L1304
  // create a predicate formatter that can be used to run the matcher
  auto pred_formatter = ::testing::internal::MakePredicateFormatterFromMatcher(matcher);
  // where: matcher_expr is the stringized matcher (e.g., StartsWith("Hello"))
  //        exec_output is the expected output, in this case the program output   
  return pred_formatter(matcher_expr, exec_output) << "\n   Input: "
                     << prog_input;
}
 
// This macro checks if the output of an executable follows the pattern defined
// in the gMock matcher
// @param prog_name name of the executable file
// @param input     keyboard input sent to the program
// @param matcher   gMock matcher used to test the executable's output
#define ASSERT_EXECTHAT(prog_name, input, matcher) \
  EXPECT_PRED_FORMAT3(AssertExecMatcher, prog_name, input, matcher)
 
::testing::AssertionResult AssertExecExit(const char* prog_name_expr,
                                        const char* prog_input_expr,
                                        const char* prog_max_dur,
                                        std::string prog_name, 
                                        std::string prog_input, 
                                        int max_dur) {
  if ( access( prog_name.c_str(), F_OK ) == -1 ) {
    return ::testing::AssertionFailure() << "      cannot test '" << prog_name 
                                         << "': Make sure your executable file"
                                         << " is called '" << prog_name << "'";
  }
 
  std::promise<bool> completed;
  auto stmt_future = completed.get_future();
  std::thread([&](std::promise<bool>& completed) {
    exec_program(prog_name, prog_input);
    completed.set_value(true);
  }, std::ref(completed)).detach();
  if(stmt_future.wait_for(std::chrono::seconds(max_dur)) == std::future_status::timeout) {
    return ::testing::AssertionFailure()
           << "Input: " << prog_input
           << "\n      The program took more than " << max_dur
           << " seconds to exit. Check for infinite loops or "
           << "unnecessary inputs.";
  } else {
    return ::testing::AssertionSuccess();
  }
}
 
// This macro checks whether the executable program exits given the provided
// input.
//
// @param prog_name name of the executable file
// @param input     keyboard input sent to the program
// @param duration  time in seconds to wait for program to exit, otherwise it is
//                  considered to have an infinite loop or requires addiitonal
//                  input
#define ASSERT_EXECEXIT(prog_name, input, duration) \
    EXPECT_PRED_FORMAT3(AssertExecExit, prog_name, input, duration)
 
 
// Version of ASSERT_EXECIO_EQ that uses google mock's matchers
//
// @param prog_name name of the executable file
// @param input     keyboard input sent to the program
// @param output    expected output of the program
#define ASSERT_EXECIO_THAT(prog_name, input, matcher) {\
  if ( access( prog_name, F_OK ) == -1 ) { \
    GTEST_FATAL_FAILURE_("      cannot test '" prog_name "': no such file"); \
  } \
  ASSERT_THAT(main_output(prog_name, input), matcher) << "   Input: " << input; \
}
 
// This macro simulates standard input and output when calling a given statement.
// The `input` parameter is routed to the simulated standard input stream (`std::cin`)
// so that the given statement receives it.
// The check block can contain statements that have access to the statement's output on the
// standard output (`std::cout`) using either the `SIO_OUT` or `your_output variables`.
//
// @param input     input value
// @param stmt      statement(s) performed
// @param check     code block that has access to the programs output to `std::cout`
#define SIMULATE_SIO(input, stmt, check) { \
  std::stringstream input_ss, output_ss; \
  auto old_inputbuf = std::cin.rdbuf(input_ss.rdbuf()); \
  auto old_outputbuf = std::cout.rdbuf(output_ss.rdbuf()); \
  input_ss.str(input); \
  stmt; \
  std::cin.rdbuf(old_inputbuf); \
  std::cout.rdbuf(old_outputbuf); \
  const std::string SIO_OUT = output_ss.str(); \
  const std::string SIO_IN = input, your_output = SIO_OUT; \
  check; \
}
 
// Version of ASSERT_SIO_EQ that uses google mock's matchers
//
// @param expected  expected string value
// @param stmt      statement(s) performed
// deprecated by SIMULATE_SIO
#define ASSERT_SIO_THAT(input, expected, stmt) { \
  std::stringstream input_ss, output_ss; \
  auto old_inputbuf = std::cin.rdbuf(input_ss.rdbuf()); \
  auto old_outputbuf = std::cout.rdbuf(output_ss.rdbuf()); \
  input_ss.str(input); \
  stmt; \
  std::cin.rdbuf(old_inputbuf); \
  std::cout.rdbuf(old_outputbuf); \
  std::string your_output = output_ss.str(); \
  ASSERT_THAT(your_output, expected); \
}
 
// This macro checks whether a function executes within a given time
//
// A thread is created to run the statement and update the status of a promise
// object. A future object is created from the promise object to check whether
// the promise  object's value was updated within the specified duration. If the
// promise's value is not changed in time, the function is considered to have
// timed out.
//
// Code based on: http://antonlipov.blogspot.com/2015/08/how-to-timeout-tests-in-gtest.html
//
// @param secs  seconds to wait before statement is considered to have
//              timed out
// @param stmt  statement to be tested
#define ASSERT_DURATION_LE(secs, stmt) { \
  std::promise<bool> completed; \
  auto stmt_future = completed.get_future(); \
  std::thread([&](std::promise<bool>& completed) { \
    stmt; \
    completed.set_value(true); \
  }, std::ref(completed)).detach(); \
  if(stmt_future.wait_for(std::chrono::seconds(secs)) == std::future_status::timeout) \
    if (!::testing::Test::HasFatalFailure()) { \
      GTEST_FATAL_FAILURE_("       Your program took more than " #secs \
      " seconds to exit. Check for infinite loops or unnecessary inputs."); \
    } \
    if (::testing::Test::HasFatalFailure()) FAIL(); \
}
