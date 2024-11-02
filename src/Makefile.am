AM_CFLAGS = -Wall -Wextra -Werror -g
AM_YFLAGS = -d

bin_PROGRAMS = vinumc

BUILT_SOURCES = dry_bison.h

vinumc_SOURCES = \
		 ast.c \
		 ast_bison_helpers.c \
		 dry_bison.y \
		 dry_flex.l \
		 vinumc.c
