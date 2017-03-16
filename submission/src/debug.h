#pragma once

#if DEBUG || ENABLE_ASSERTION

#define ASSERT(expr, ...)           AssertWithMessage(!!(expr), #expr, __PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define ASSERT_UNREACHABLE(...)     AssertWithMessage(false, "Bug: should not reach here", __PRETTY_FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

static void AssertWithMessage(
    __in     bool exprCond,
    __in     const char* expression,
    __in     const char* function,
    __in     const char* filePath,
    __in     int lineNo,
    __in_opt const char* message = nullptr,
    __in_opt ...
) {
    __thread static char fmtMsg[4096];

    if (!exprCond) {
        if (message == nullptr) {
            // Set to empty string to prevent a "(null)" output
            message = "";
        }
        else {
            va_list args;
            va_start(args, message);
            vsprintf(fmtMsg, message, args);
            va_end(args);
        }

        const char* fileName = std::max(strrchr(filePath, '/'), strrchr(filePath, '\\'));
        if (fileName == nullptr) {
            fileName = filePath;
        }
        else {
            ++fileName;
        }
        fprintf(stderr,
            "\n"
            "================ ASSERTION FAILED ================\n"
            "\n"
            "  Expression | %s\n"
            "     Message | %s\n"
            "    Function | %s\n"
            "    Location | %s (Line: %d)\n"
            "\n",
            expression, fmtMsg, function, fileName, lineNo
        );
        fflush(stderr);

        abort();
    }
}

#else

#define ASSERT(expr, ...)           __assume(expr)
#define ASSERT_UNREACHABLE(...)     __assume(0)

#endif // DEBUG || ENABLE_ASSERTION
