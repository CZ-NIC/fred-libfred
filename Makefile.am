ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
		$(BOOST_CPPFLAGS) \
		-I${srcdir}

AM_LDFLAGS = $(BOOST_LDFLAGS)

AM_CXXFLAGS = \
		-fdiagnostics-color=auto \
		-Wall \
		-Wextra \
		-ggdb \
		-O2

SUBDIRS = .

lib_LTLIBRARIES = libfred.la

libfred_la_SOURCES = \
	src/libfred/registrable_object/contact/info_contact.cc \
	src/libfred/registrable_object/contact/info_contact.hh

libfred_la_LDFLAGS = -version-info 0:0:0

#libfred_la_LDADD = \
#		$(BOOST_SYSTEM_LIB)
