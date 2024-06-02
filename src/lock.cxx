#include "lock.h"

namespace cards {
lock::lock(const std::string& m_root, bool exclusive)
	: m_dir(0)
{
  // Ignore signals
  memset(&m_sa, 0, sizeof(m_sa));
  m_sa.sa_handler = SIG_IGN;
  sigaction(SIGHUP, &m_sa, NULL);
  sigaction(SIGINT, &m_sa, NULL);
  sigaction(SIGQUIT, &m_sa, NULL);
  sigaction(SIGTERM, &m_sa, NULL);

	const std::string dirname = trimFileName(m_root + std::string("/") + PKG_DB_DIR);
#ifndef NDEBUG
	std::cerr << "Lock the database " << dirname << std::endl;
#endif
	if (!(m_dir = opendir(dirname.c_str())))
		throw RunTimeErrorWithErrno("could not read directory " + dirname);

	if (flock(dirfd(m_dir), (exclusive ? LOCK_EX : LOCK_SH) | LOCK_NB) == -1) {
		if (errno == EWOULDBLOCK)
			throw std::runtime_error("package database is currently locked by another process");
		else
			throw RunTimeErrorWithErrno("could not lock directory " + dirname);
	}
}

lock::~lock()
{
	m_sa.sa_handler = SIG_DFL;
	signal(SIGHUP,SIG_DFL);
	signal(SIGINT,SIG_DFL);
	signal(SIGQUIT,SIG_DFL);
	signal(SIGTERM,SIG_DFL);
	if (m_dir) {
		flock(dirfd(m_dir), LOCK_UN);
		closedir(m_dir);
	}
#ifndef NDEBUG
	std::cerr << "Unlock the database " << m_dir << std::endl;
#endif
}
} // end of cards namespace

// vim:set ts:2 :
