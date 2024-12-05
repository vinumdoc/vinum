AM_CFLAGS = -Wall -Wextra -Werror -g
AM_YFLAGS = -d

bin_PROGRAMS = vinumc sweet_vinumc

BUILT_SOURCES = dry_bison.h sweet_bison.h

vinumc_SOURCES = \
		 ast.c \
		 ast_bison_helpers.c \
		 dry_bison.y \
		 dry_flex.l \
		 eval.c \
		 vinumc.c

sweet_vinumc_SOURCES = \
		 sweet_bison.y \
		 sweet_flex.l \
		 sweet_vinumc.c
