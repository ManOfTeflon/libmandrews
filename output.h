#ifndef MACRO_H_
#define MACRO_H_

#include <iostream>
#include <sstream>
#include <set>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <readline/readline.h>
#include <readline/history.h>

#include "macro.h"

#define COMPILE_ASSERT(condition) \
    enum { assert_static__ = 1/(condition) }
#define ILLEGAL_DEFAULT() default: \
    P(FTL) << "Illegal default case reached"

enum Channel { OUT, ERR, FTL, BRK, DBG, DMY, BUF };

namespace logging {

const std::string endl = "\n";

#ifndef NVERBOSE
#define VERBOSE
#endif
#ifndef NDEBUG
#define DEBUG
#endif
#ifndef NCAUTIOUS
#define CAUTIOUS
#endif

#define V(v)  #v << ": " << v << " "

#define P(channel) ::logging::Dump(channel, __FILE__, __LINE__)
#define PIPE(name, channel)  ::logging::Dump name(channel, __FILE__, __LINE__)

#define DISABLED_STREAM if(false) std::cftl

#ifdef DEBUG
#define D(channel) P(channel)
#else
#define D(channel) DISABLED_STREAM
#endif

#define EXPECT(condition) \
    if(!(condition)) D(BRK) << "Failed assertion: " #condition << ::logging::endl
#ifdef CAUTIOUS
#define ASSERT(condition) EXPECT(condition)
#else
#define ASSERT(condition) DISABLED_STREAM
#endif

#define CHOICE(t, p, o) \
  ::logging::Choice<t>((p), (o), (::logging::isNull(o)) ? ARRAYSIZE(o) : 0)
#define PRINT(x)  D(OUT) << (#x ": ") << x
#define ASSERT_EQ(x, y) ASSERT((x) == (y)) << (x) << " vs. " << (y) \
    << ::logging::endl
#define ASSERT_NE(x, y) ASSERT((x) != (y)) << (x) << " vs. " << (y) \
    << ::logging::endl

constexpr bool isNull(char ary[]) {
  return ary;
}

template<bool FTL = false>
class NulStreambuf : public std::streambuf {
    char dummyBuffer[64];
protected:
    virtual int overflow( int c ) {
        assert(!FTL);
        setp( dummyBuffer, dummyBuffer + sizeof( dummyBuffer ) ) ;
        return (c == EOF) ? '\0' : c ;
    }
};

class NulOStream : public NulStreambuf<false>, public std::ostream {
public:
    NulOStream() : std::ostream( this ) {}
};

class FtlOStream : public NulStreambuf<true>, public std::ostream {
public:
    FtlOStream() : std::ostream( this ) {}
};

}

namespace std { static ::logging::NulOStream cnul; }
namespace std { static ::logging::FtlOStream cftl; }

namespace logging {

struct ChannelMetadata {
  Channel channel;
  std::ostream* os;
  const char* color;
};

constexpr ChannelMetadata channel_map[] = {
    { OUT, &std::cout, "\E[1;35m" },
    { ERR, &std::cerr, "\E[1;31m" },
    { FTL, &std::cerr, "\E[1;31m" },
    { BRK, &std::cerr, "\E[1;31m" },
    { DBG, &std::cout, "\E[1;35m" },
    { DMY, &std::cnul, "\E[0;35m" },
    { BUF, &std::cftl, "\E[0;34m" }
};

inline constexpr ChannelMetadata const* channelMetadata(Channel channel, ChannelMetadata const* map) {
  return channel == map->channel ? map : channelMetadata(channel, map + 1);
}

inline constexpr std::ostream* channelStream(Channel channel) {
  return channelMetadata(channel, channel_map)->os;
}

inline constexpr const char* channelColor(Channel channel) {
  return channelMetadata(channel, channel_map)->color;
}

template<typename T>
class Choice {
 public:
  Choice<T>(const std::string& prompt, T* options, int num_options) :
      options_(options, options + num_options) {
    std::cout << prompt << " (";
    for (int i = 0; i < num_options; ++i) {
      if (i) std::cout << "/";
      std::cout << options[i];
    }
    std::cout << ") ";
  }

  Choice<T>& operator>>(T& t) {
    do {
      std::cin >> t;
    } while (options_.find(t) == options_.end());
    return *this;
  }

 private:
  std::set<T> options_;
};

class Prompt {
 public:
  Prompt(const std::string& prompt) {
    add_history(line = readline(prompt.c_str()));
  }
  ~Prompt() {
    free(line);
  }

  Prompt& operator>>(std::string& t) {
    t = line;
    return *this;
  }

  operator const char*() {
      return line;
  }
 private:
  char* line;
};

class Dump {
 public:
  Dump(Channel channel, const std::string& file, int line) :
    channel_(channel),
    file_(file),
    line_(line),
    color_(channelColor(channel)),
    os_(channelStream(channel))
  {
    assert(channel != BUF);
  }

  Dump(std::ostream& os, const std::string& file, int line) :
    channel_(BUF),
    file_(file),
    line_(line),
    color_(channelColor(BUF)),
    os_(&os) { }

  Dump(const Dump& other) :
    channel_(other.channel_),
    file_(other.file_),
    line_(other.line_),
    color_(other.color_),
    ss_(other.ss_.str()),
    os_(other.os_) { }

  virtual ~Dump() {
    flush();
  }

  inline void flush() {
    switch(channel_) {
      case OUT:
        Output("OUT");
        break;
      case ERR:
        Output("ERR");
        break;
      case FTL:
        Output("FTL");
        abort();
        break;
      case BRK:
       {
        Output("BRK");
        char choice;
        char options[] = {'y', 'n'};
        CHOICE(char, "Continue?", options) >> choice;
        if (choice == 'n') abort();
        break;
       }
      case BUF:
        Output("BUF");
        break;
      case DBG:
#ifdef DEBUG
        Output("DBG");
        break;
#endif
      case DMY:
      default:
        break;
    }
    ss_.clear();
    ss_.str(std::string());
  }

  template<class T>
  Dump& operator<<(const T& t) {
    ss_ << t;
    return *this;
  }

  Dump& operator>>(std::string& s) {
    s = ss_.str();
    return *this;
  }

  static int Indent() {
      return ++Indentation();
  }

  static int Unindent() {
      int& i = Indentation();
      if (i) --i;
      return i;
  }

  inline bool expect(const std::string& expected) {
      return !expected.compare(ss_.str());
  }

 protected:
  inline void Output(const char* prefix) {
#ifdef VERBOSE
    char prompt[maxTotalLength];
    char filename_buffer[filenameLength + 1];
    const char* filename = Filename(filename_buffer, filenameLength + 1);
    int written = snprintf(prompt, maxTotalLength, "%s%s (%*.*s%s)\E[0m%*s",
            color_.c_str(), prefix, filenameLength, filenameLength,
            filename, color_.c_str(), Indentation() * spacesPerIndent, "");
    assert(written < maxTotalLength);
    int real_len = written - color_.size() * 5 - 4;
    Print(*os_, real_len, prompt);
#else
    char empty[] = { '\0' };
    Printf(*os_, 0, empty);
#endif
  }

  inline void Print(std::ostream& stream, int prefix, char* prompt) {
    const int prompt_len = strlen(prompt);
    char buffer[maxTotalLength + 1] = { '\0' };
#ifdef VERBOSE
    const std::string carat = "\E[1;32m>\E[0m ";
    bool first = true;
    while(ss_.get(buffer, maxTotalLength + 1, '\n').gcount()) {
      ss_.ignore();
      stream << prompt << carat << buffer << std::endl;
      if (first) {
        prompt += prompt_len - prefix;
        memset(prompt, ' ', prefix);
        first = false;
      }
    }
#else
    while(ss_.get(buffer, maxTotalLength + 1, '\n').gcount()) {
      ss_.ignore();
      stream << buffer << std::endl;
    }
#endif
  }

  const Channel channel_;
  const std::string file_;
  const int line_;
  const std::string color_;
  std::stringstream ss_;
  std::ostream* os_;
  static constexpr int filenameLength = 40;
  static constexpr int maxTotalLength = 128;
  static constexpr int spacesPerIndent = 2;

  static inline int& Indentation() {
    static int indentation = 0;
    return indentation;
  }
  const char* Filename(char* buffer, int length) const {
    const char* bold = "\E[1;36m";
    const char* colon = "\E[0;35m:";
    const int line_len = 4;
    const int file_len = length - line_len - strlen(colon) - 2 * strlen(bold) - 1;
    const char* file = file_.c_str();
    const char* f = file + strlen(file);
    int i = 0;
    for (; (f - 1) >= file && *(f - 1) != '/'; ++i, --f);
    if (i > file_len) f += (i - file_len);
    int written = snprintf(buffer, length, "%s%*s%s%s%*d", bold, file_len, f, colon, bold, line_len, line_);
    assert(written == length - 1);
    if (i > file_len) memset(buffer, '.', 3);
    return buffer;
  }
};

#define STATIC_CALL(msg) \
    namespace { \
    namespace CONCAT(static_init__, __LINE__) { \
    class Init { \
     public: \
      Init() { \
        const ::std::string message = (msg); \
        if(!message.empty()) D(DBG) << "Running " << message; \
        bool success = Run(); \
        if(!message.empty() && success) D(DBG) << message << " successful."; \
        if(!message.empty() && !success) D(BRK) << message << " failed!"; \
      } \
      bool Run(); \
    }; \
    Init init; \
    } \
    } \
    bool CONCAT(static_init__, __LINE__)::Init::Run()

#define STATIC_INIT(msg) \
    STATIC_CALL("Static Initialization(\E[0;36m" msg "\E[0m)")

}  // namespace logging

template<typename T>
struct SingleIter {
    template<typename ... Args>
    SingleIter(Args...args) : t{ T(args...) } { }

    T* begin() { return t; }
    T* end() { return t + 1; }

private:
    T t[1];
};

#define CAPTURE(name, stream) \
    for (::logging::Dump& name : SingleIter<::logging::Dump>(stream, __FILE__, __LINE__))

template<typename ... Args>
struct variadic { };

#endif  // MACRO_H_
