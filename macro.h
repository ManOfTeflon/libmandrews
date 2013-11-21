#ifndef MACRO_H_
#define MACRO_H_

#include <iostream>
#include <sstream>
#include <set>
#include <assert.h>
#include <stdlib.h>

#define SUBSETS(i, s)		for(uint64 i = 0ull, null_flag = 1ull; \
                            i || null_flag; \
                            i = (i-1) & (s), null_flag = 0ull)
#define STEP(i, l, h, s)	for(int i=l;i<h;i+=s)
#define ISTEP(i, l, h, s)	for(int i=h - 1;i >= l;i-=s)
#define FOR(i, l, h)		STEP(i, l, h, 1)
#define IFOR(i, l, h)		ISTEP(i, l, h, 1)
#define REP(i, h)		FOR(i, 0, h)
#define IREP(i, h)		IFOR(i, 0, h)
#define RANGE(p, l, h)		(p >= l && p < h)
#define IN(i, b)		for(Board i = least(b);i;b^=i,i=least(b))
#define SZ(x) 			((int)x.length())

#define INF			32767

#define COMPILE_ASSERT(condition) \
    do { \
        enum { assert_static__ = 1/(condition) }; \
    } while (0)
#define ILLEGAL_DEFAULT() default: \
    P(FTL) << "Illegal default case reached"

#define ARRAYSIZE(ary) (sizeof(ary) / sizeof(*ary))

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

enum Channel { OUT, ERR, FTL, BRK, DBG, DMY, BUF };

namespace logging {

const std::string endl = "\n";

#define DEBUG
#define CAUTIOUS

#define V(v)  #v << ": " << v << " "

#define P(channel) ::logging::Dump(channel, __FILE__, __LINE__)
#define PIPE(name, channel)  ::logging::Dump name(channel, __FILE__, __LINE__)

#ifdef DEBUG
#define D(channel) P(channel)
#else
#define D(channel) if(false) std::cout
#endif

#ifdef CAUTIOUS
#define ASSERT(condition) \
  ::logging::Assert(condition, #condition, __FILE__, __LINE__)
#else
#define ASSERT(condition) if(false) std::cout
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

class NulStreambuf : public std::streambuf {
    char dummyBuffer[64];
protected:
    virtual int overflow( int c ) {
        setp( dummyBuffer, dummyBuffer + sizeof( dummyBuffer ) ) ;
        return (c == EOF) ? '\0' : c ;
    }
};

class NulOStream : public NulStreambuf, public std::ostream {
public:
    NulOStream() : std::ostream( this ) {}
};

}

namespace std { static ::logging::NulOStream cnul; }

namespace logging {

struct ChannelMetadata {
  Channel channel;
  std::ostream& os;
  const char* color;
};

constexpr ChannelMetadata channel_map[] = {
    { OUT, std::cout, "\E[1;35m" },
    { ERR, std::cerr, "\E[1;31m" },
    { FTL, std::cerr, "\E[1;31m" },
    { BRK, std::cerr, "\E[1;31m" },
    { DBG, std::cout, "\E[1;35m" },
    { DMY, std::cnul, "\E[0;35m" },
    { BUF, std::cnul, "\E[0;34m" }
};

inline constexpr ChannelMetadata const* channelMetadata(Channel channel, ChannelMetadata const* map) {
  return channel == map->channel ? map : channelMetadata(channel, map + 1);
}

inline constexpr std::ostream& channelStream(Channel channel) {
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
    std::cout << prompt << " ";
  }

  template<typename T>
  Prompt& operator>>(T& t) {
    std::cin >> t;
    return *this;
  }
};

class Dump {
 public:
  Dump(Channel channel, const std::string& file, int line) :
    channel_(channel),
    file_(file),
    color_(channelColor(channel)),
    line_(line),
    os_(channelStream(channel))
  {
    assert(channel != BUF);
  }

  Dump(std::ostream& os, const std::string& file, int line) :
    channel_(BUF),
    file_(file),
    color_(channelColor(BUF)),
    line_(line),
    os_(os) { }

  virtual ~Dump() {
    flush();
  }

  void flush() {
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

 private:
  void Output(const std::string& prefix) {
    std::string filename = file_.substr(file_.find_last_of("/") + 1);
    std::stringstream prompt;
    prompt << color_ << prefix << color_ << " (\E[1;36m"
        << filename << "\E[0;35m:\E[1;36m" << line_
        << color_ << ")\E[0m";
    Print(os_, prefix.size(), prompt.str());
  }

  void Print(std::ostream& stream, int prefix, std::string prompt) {
    const int line_length = 128;
    char buffer[line_length + 1] = { '\0' };
    const std::string carat = "\E[1;32m>\E[0m ";
    bool first = true;
    while(ss_.getline(buffer, line_length + 1).gcount()) {
      stream << prompt << (first ? carat : "  ") << buffer << std::endl;
      if (first) {
        prompt = prompt.replace(0, prefix + color_.size(), std::string(prefix, ' '));
        first = false;
      }
    }
  }

  const Channel channel_;
  const std::string file_;
  const std::string color_;
  const int line_;
  std::stringstream ss_;
  std::ostream& os_;
};

class Assert {
 public:
  explicit Assert(bool condition, const std::string& msg,
                  const std::string& file, int line) :
    condition_(condition),
    msg_(msg),
    file_(file),
    line_(line) {
  }

  virtual ~Assert() {
    if(!condition_) {
      if(!ss_.str().empty()) {
        Dump(ERR, file_, line_) << "Assertion failed: " << ss_.str();
      }
      Dump(BRK, file_, line_) << "False condition: " << msg_;
    }
  }

  template<class T>
  Assert& operator<<(const T& t) {
    ss_ << t;
    return *this;
  }

 private:
  const bool condition_;
  const std::string& msg_;
  const std::string& file_;
  int line_;
  std::stringstream ss_;
};

#define STATIC_CALL(msg) \
    namespace { \
    namespace CONCAT(static_init__, __LINE__) { \
    class Init { \
     public: \
      Init() { \
        const ::std::string message = (msg); \
        if(!message.empty()) D(DBG) << "Running " << message; \
        Run(); \
        if(!message.empty()) D(DBG) << message << " successful."; \
      } \
      void Run(); \
    }; \
    extern Init init; \
    } \
    } \
    void CONCAT(static_init__, __LINE__)::Init::Run()

#define STATIC_INIT(msg) \
    STATIC_CALL("Static Initialization(\E[0;36m" msg "\E[0m)")

}  // namespace logging

#endif  // MACRO_H_
