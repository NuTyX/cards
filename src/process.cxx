/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Johannes Winkelmann, jw@tks6.net
//  Output redirection by Logan Ingalls, log@plutor.org
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "process.h"

process::process()
{
}
process::process(const std::string& app, const std::string& arguments, int fdlog)
    : m_application(app)
    , m_arguments(arguments)
    , m_fileDescriptorLog(fdlog)
{
}
void process::execute(const std::string& app, const std::string& arguments, int fdlog)
{
    m_application = app;
    m_arguments = arguments;
    m_fileDescriptorLog = fdlog;
#ifdef DEBUG
    std::cerr << m_application << " " << m_arguments << " start" << std::endl;
#endif

    std::cout << "  " << m_application << " " << m_arguments << std::endl;
    execute();

#ifdef DEBUG
    std::cerr << m_application << " " << m_arguments << " end" << std::endl;
#endif
}
std::string process::name()
{
    return m_application;
}
std::string process::args()
{
    return m_arguments;
}
int process::execute()
{
    std::list<std::string> args;
    split(m_arguments, ' ', args, 0, false);

    const int argc = 1 + args.size() + 1; // app, args, NULL

    char** argv = new char*[argc];
    std::list<std::string>::iterator it = args.begin();

    int i = 0;
    argv[i] = const_cast<char*>(m_application.c_str());
    for (; it != args.end(); ++it) {
        ++i;
        argv[i] = const_cast<char*>(it->c_str());
    }

    ++i;
    assert(i + 1 == argc);
    argv[i] = NULL;
    int status = 0;

    if (m_fileDescriptorLog > 0) {
        status = execLog(argc, argv);
    } else {
        status = exec(argc, argv);
    }
    delete[] argv;
    return status;
}

int process::execLog(const int argc, char** argv)
{
    int status = 0;
    int fdpipe[2];
    pipe(fdpipe);

    pid_t pid = fork();
    if (pid == 0) {
        // child process
        close(fdpipe[0]);
        dup2(fdpipe[1], STDOUT_FILENO);
        dup2(fdpipe[1], STDERR_FILENO);

        execv(m_application.c_str(), argv);
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // fork failed
        if (status == 0)
            status = -1;
    } else {
        // parent process
        close(fdpipe[1]);

        char readbuf[1024];
        int bytes, wpval;

        while ((wpval = waitpid(pid, &status, WNOHANG)) == 0) {
            while ((bytes = read(fdpipe[0], readbuf, sizeof(readbuf) - 1)) > 0) {
                readbuf[bytes] = 0;
                printf("%s", readbuf);
                fflush(stdout);
                fflush(stderr);
                write(m_fileDescriptorLog, readbuf, bytes);
            }
        }
        close(fdpipe[0]);

        if (wpval != pid) {
            if (status == 0)
                status = -1;
        }
    }
    return status / 256;
}

int process::exec(const int argc, char** argv)
{
    int status = 0;
    pid_t pid = fork();
    if (pid == 0) {
        // child process
        execv(m_application.c_str(), argv);
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // fork failed
        if (status == 0)
            status = -1;
    } else {
        // parent process
        if (waitpid(pid, &status, 0) != pid) {
            if (status == 0)
                status = -1;
        }
    }
    return status / 256;
}

int process::executeShell()
{
    // TODO: make shell exchangable
    static const char SH[] = "/bin/sh";
    int status = 0;
    if (m_fileDescriptorLog > 0) {
        status = execShellLog(SH);
    } else {
        status = execShell(SH);
    }
    return status;
}

int process::execShellLog(const char* SH)
{
    int status = 0;

    int fdpipe[2];
    pipe(fdpipe);

    pid_t pid = fork();
    if (pid == 0) {
        // child process
        close(fdpipe[0]);
        dup2(fdpipe[1], STDOUT_FILENO);
        dup2(fdpipe[1], STDERR_FILENO);

        execl(SHELL, SHELL, "-c", (m_application + " " + m_arguments).c_str(), NULL);
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // fork failed
        if (status == 0)
            status = -1;
    } else {
        // parent process
        close(fdpipe[1]);

        char readbuf[1024];
        int bytes, wpval;

        while ((wpval = waitpid(pid, &status, WNOHANG)) == 0) {
            while ((bytes = read(fdpipe[0], readbuf, sizeof(readbuf) - 1)) > 0) {
                readbuf[bytes] = 0;
                printf("%s", readbuf);
                fflush(stdout);
                fflush(stderr);
                write(m_fileDescriptorLog, readbuf, bytes);
            }
        }
        close(fdpipe[0]);

        if (wpval != pid) {
            if (status == 0)
                status = -1;
        }
    }
    return status / 256;
}

int process::execShell(const char* shell)
{
    int status = 0;

    pid_t pid = fork();
    if (pid == 0) {
        execl(shell, shell, "-c", (m_application + " " + m_arguments).c_str(), NULL);
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // fork failed
        if (status == 0)
            status = -1;
    } else {
        // parent process
        if (waitpid(pid, &status, 0) != pid) {
            if (status == 0)
                status = -1;
        }
    }
    return status / 256;
}
