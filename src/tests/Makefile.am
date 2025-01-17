AM_CFLAGS = -Wall -Wextra -Werror -Wformat-security -g

TESTS =

bin_PROGRAMS = $(TESTS)

LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/build-aux/tap-driver.sh
