AM_CFLAGS = -Wall -Wextra -Werror -Wformat-security -g
AM_YFLAGS = -d

bin_PROGRAMS = vinumc vin2dot

BUILT_SOURCES = dry_bison.h

COMMON_SOURCES = \
		 ast.c \
		 ast_bison_helpers.c \
		 dry_bison.y \
		 dry_flex.l \
		 eval.c

vinumc_SOURCES = \
		 $(COMMON_SOURCES) \
		 vinumc.c

vin2dot_SOURCES = \
		 $(COMMON_SOURCES) \
		 vin2dot.c
