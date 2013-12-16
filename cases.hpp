
namespace Tester {

inline const Case& Case::Fork(int i) {
    // stderr is also re-directed to stdout.  This is the only way to get
    // anywhere close to the right output order from the forked process.
    // In fact, it will guarantee exact output order.
    int out[2];
    SysCall(pipe2)(out, O_NONBLOCK);
    _start = now();
    int pid = SysCall(fork)();
    if (pid) {
        Run::Parent() = true;
        SysCall(close)(out[WRITE_END]);
        TestProcess* p = new TestProcess{i, pid, out[READ_END]};
        p->f = SysCall(open_memstream)((char**)(&p->buf.iov_base), &p->buf.iov_len);
        _children.insert(std::pair<int, TestProcess*>(pid, p));
    } else {
        Run::Parent() = false;
        SysCall(close)(out[READ_END]);
        SysCall(dup2)(out[WRITE_END], STDOUT_FILENO);
        SysCall(dup2)(STDOUT_FILENO, STDERR_FILENO);
        Call(i);
    }
    return *this;
}

inline const Case& Case::ForkAll() {
    int n = Num();
    for (int i = 0; i < n; ++i) {
        Fork(i);
        if (!Run::Parent()) break;
    }
    return *this;
}

inline bool Case::WaitAll() {
    bool success = true;
    while (!_children.empty()) {
        for (auto it = _children.begin(); it != _children.end(); ++it) {
            Result r;
            Flush(*it->second);
            if ((r = WaitPid(*it->second)) != Result::RUNNING) {
                success &= (r == Result::SUCCESS);
                _children.erase(it);
            }
        }
    }
    ASSERT(success);
    return success;
}

inline void Case::Flush(TestProcess& child) {
    const int block_size = 4096;
    char buf[block_size];
    while (true) {
        int r = read(child.out, buf, block_size);
        int err = errno;
        if (r == -1 && err == 11) continue;
        else if (r == 0) break;
        ASSERT(r > 0) << "Failed to read from child " << child.i << " pipe:"
            " (" << err << ")" << strerror(err);
        SysCall(fwrite)(buf, r, 1, child.f);
    }
}

inline Case::Result Case::WaitPid(TestProcess& child) {
    int status;
    ASSERT(Run::Parent());
    int reaped;
    reaped = SysCall(waitpid)(child.pid, &child.status, (_timeout > 0) ? WNOHANG : 0);
    auto n = now();
    child.usecs = n - _start;
    if (!reaped && child.usecs < _timeout * US_PER_SEC) return Result::RUNNING;
    else if (reaped) ASSERT(reaped == child.pid) << "Reaped wrong child!  " << V(reaped) << V(child.pid);
    SysCall(fclose)(child.f);
    SysCall(close)(child.out);
    fprintf(stdout, "\n");
    P(OUT) << "Beginning test '\E[0;36m" << _name << "\E[0m' "
        "with arguments '\E[0;36m" << ArgName(child.i) << "\E[0m'";
    std::cout.flush();
    SysCall(fflush)(stdout);
    Auto auto_free([&child]{ free(child.buf.iov_base); });
    SysCall(fwrite)(child.buf.iov_base, child.buf.iov_len, 1, stdout);
    if (!reaped) {
        // Ain't got time fo' that.
        kill(child.pid, 9);
        return Finish(child, Result::TIMEOUT);
    } else if (status != _expected) {
        return Finish(child, Result::FAILURE);
    } else {
        return Finish(child, Result::SUCCESS);
    }
}

inline Case::Result Case::Finish(const TestProcess& child, Case::Result result) {
    const char* args = ArgName(child.i);
    switch (result) {
        case Result::SUCCESS: {
            PIPE(r, OUT);
            r << "Test '\E[0;36m" << _name << "\E[0m' succeeded";
            if (strlen(args)) r << " with arguments '\E[0;36m" << args << "\E[0m'";
            r << "\n\tin \E[1;34m" << ((double)(child.usecs) / 1000) << "\E[0m milliseconds "
                << "with exit code \E[1;34m" << child.status << "\E[0m!";
            break;
        }
        case Result::FAILURE: {
            PIPE(r, ERR);
            r << "Test '\E[0;36m" << _name << "\E[0m' \E[1;31mfailed\E[0m";
            if (strlen(args)) r << " with arguments '\E[0;36m" << args << "\E[0m'";
            r << "\n\tin \E[1;34m" << ((double)(child.usecs) / 1000) << "\E[0m milliseconds "
                << " with exit code \E[1;14m" << child.status << "\E[0m!\n\t"
                "Expected \E[1;34m" << _expected << "\E[0m.";
            break;
        }
        case Result::TIMEOUT: {
            PIPE(r, ERR);
            r << "Test '\E[0;36m" << _name << "\E[0m' \E[1;31mtimed out\E[0m";
            if (strlen(args)) r << " with arguments '\E[0;36m" << args << "\E[0m'";
            r << "\n\tin \E[1;34m" << ((double)(child.usecs) / 1000) << "\E[0m milliseconds ";
            break;
        }
        ILLEGAL_DEFAULT();
    }
    return result;
}

inline void Case::Succeed(uint64_t usecs) {
    PIPE(r, OUT);
    r << "Test '\E[0;36m" << _name << "\E[0m' succeeded";
    r << "\n\tin \E[1;34m" << ((double)(usecs) / 1000) << "\E[0m milliseconds";
}

inline Run::Run(std::initializer_list<Case*> cases) {
    for (Case* c : cases) {
        c->ForkAll();
        if (Parent()) {
            c->WaitAll();
        }
        else break;
    }
    if (Parent()) {
        printf("\n");
        exit(0);
    }
}

}
