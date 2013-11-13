#ifndef MACRO_H_
#define MACRO_H_

#include <iostream>
#include <sstream>
#include <set>
#include <assert.h>
#include <stdlib.h>

using namespace std;

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

enum Channel { OUT, ERR, FTL, BRK, DBG, DMY };

namespace logging {

const string endl = "\n";

#define DEBUG
#define CAUTIOUS

#define V(v)  #v << ": " << v << " "

#define P(channel) ::logging::Dump(channel, __FILE__, __LINE__)
#define O(channel, name)  ::logging::Dump name(channel, __FILE__, __LINE__)

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

template<typename T>
class Choice {
 public:
  Choice<T>(const string& prompt, T* options, int num_options) :
      options_(options, options + num_options) {
    cout << prompt << " (";
    for (int i = 0; i < num_options; ++i) {
      if (i) cout << "/";
      cout << options[i];
    }
    cout << ") ";
  }

  Choice<T>& operator>>(T& t) {
    do {
      cin >> t;
    } while (options_.find(t) == options_.end());
    return *this;
  }

 private:
  set<T> options_;
};

class Prompt {
 public:
  Prompt(const string& prompt) {
    cout << prompt << " ";
  }

  template<typename T>
  Prompt& operator>>(T& t) {
    cin >> t;
    return *this;
  }
};

class Dump {
 public:
  Dump(Channel channel, const string& file, int line) :
    channel_(channel),
    file_(file),
    line_(line) { }

  virtual ~Dump() {
    switch(channel_) {
      case OUT:
        Output("OUT");
        break;
      case ERR:
        Error("ERR");
        break;
      case FTL:
        Error("FTL");
        abort();
        break;
      case BRK:
       {
        Error("BRK");
        char choice;
        char options[] = {'y', 'n'};
        CHOICE(char, "Continue?", options) >> choice;
        if (choice == 'n') abort();
        break;
       }
      case DBG:
#ifdef DEBUG
        Output("DBG");
        break;
#endif
      case DMY:
      default:
        break;
    }
  }

  void Output(const string& prefix) {
    string filename = file_.substr(file_.find_last_of("/") + 1);
    stringstream prompt;
    prompt << "\E[1;35m" << prefix << "\E[0;35m (\E[1;36m"
        << filename << "\E[0;35m:\E[1;36m" << line_
        << "\E[0;35m)\E[0m";
    Print(cout, 7 + prefix.size(), prompt.str());
  }

  void Error(const string& prefix) {
    string filename = file_.substr(file_.find_last_of("/") + 1);
    stringstream prompt;
    prompt << "\E[1;31m" << prefix << "\E[0;31m (\E[1;36m"
        << filename << "\E[0;35m:\E[1;36m" << line_
        << "\E[0;31m)\E[0m";
    Print(cerr, 7 + prefix.size(), prompt.str());
  }

  void Print(ostream& stream, int prefix, string prompt) {
    const int line_length = 128;
    char buffer[line_length + 1] = { '\0' };
    const string carat = "\E[1;32m>\E[0m ";
    bool wrap = false;
    bool clear = true;
    while(ss_.get(buffer, line_length + 1).gcount()) {
      stream << prompt << (wrap ? "  " : carat) << buffer << std::endl;
      wrap = false;
      if (clear) {
        prompt = prompt.replace(0, prefix, string(prefix, ' '));
        clear = false;
      }
      if (ss_.peek() == '\n') ss_.get();
      else wrap = true;
    }
  }

  template<class T>
  Dump& operator<<(const T& t) {
    ss_ << t;
    return *this;
  }

 private:
  const Channel channel_;
  string file_;
  int line_;
  stringstream ss_;
};

class Assert {
 public:
  explicit Assert(bool condition, const string& msg,
                  const string& file, int line) :
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
  const string& msg_;
  const string& file_;
  int line_;
  stringstream ss_;
};

#define STATIC_CALL(msg) \
    namespace { \
    namespace CONCAT(static_init__, __LINE__) { \
    class Init { \
     public: \
      Init() { \
        const string message = (msg); \
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
