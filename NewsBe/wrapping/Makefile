# $Id$

# Trivial Makefile

CC	=	gcc
CFLAGS	=	-Wall -O4
LDFLAGS	=	-s

TOOLS	=	rewrap quote

all:	${TOOLS}

quote:	wrap-plain.c main.c wrappers.h
	${CC} ${CFLAGS} ${LDLAGS} -DPLAIN wrap-plain.c  main.c -o quote

rewrap:	wrap-quoted.c main.c wrappers.h
	${CC} ${CFLAGS} ${LDLAGS} -DQUOTE wrap-quoted.c main.c -o rewrap
	
clean:
	rm -f core ${TOOLS}

gz:	clean
	gtar zvcf wrapping.tar.gz `ls ./* | grep -v '\.gz$$' | grep -v '\.uue$$'`

dist:   clean
	( dir=`basename \`pwd\`` ; cd .. ;		\
	  files=`ls -dF $$dir/* $$dir/*/*	|	\
		grep -v '/$$'			|	\
		grep -v CVS` ;				\
	  tar cf $$dir.tar $$files ;			\
	  rm -f $$dir.tar.gz ;				\
	  gzip -9 $$dir.tar )

uue:    dist
	@(dir=`basename \`pwd\`` ; cd .. ;		\
	  uuencode $$dir.tar.gz $$dir.tar.gz > $$dir.uue )
