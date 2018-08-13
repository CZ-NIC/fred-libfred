ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
		$(POSTGRESQL_CFLAGS) \
		$(BOOST_CPPFLAGS) \
		-I${srcdir}

AM_LDFLAGS = \
		$(POSTGRESQL_LDFLAGS) \
		$(BOOST_LDFLAGS)

AM_CXXFLAGS = \
		-fdiagnostics-color=auto \
		-Wall \
		-Wextra \
		-ggdb \
		-O2

SUBDIRS = .

lib_LTLIBRARIES = libfred.la

libfred_la_SOURCES = \
	src/libfred/opexception.hh \
	src/libfred/opcontext.cc \
	src/libfred/opcontext.hh \
	src/libfred/db_settings.hh \
	src/libfred/registrable_object/contact/info_contact.cc \
	src/libfred/registrable_object/contact/info_contact.hh \
	src/libfred/registrable_object/contact/info_contact_data.cc \
	src/libfred/registrable_object/contact/info_contact_data.hh \
	src/libfred/registrable_object/contact/info_contact_diff.cc \
	src/libfred/registrable_object/contact/info_contact_diff.hh \
	src/libfred/registrable_object/contact/info_contact_impl.cc \
	src/libfred/registrable_object/contact/info_contact_impl.hh \
	src/libfred/registrable_object/contact/info_contact_output.cc \
	src/libfred/registrable_object/contact/info_contact_output.hh \
	src/libfred/registrable_object/contact/place_address.cc \
	src/libfred/registrable_object/contact/place_address.hh \
	src/util/base_exception.hh \
	src/util/is_equal_optional_nullable.hh \
	src/util/map_at.hh \
	src/util/optional_value.hh \
	src/util/printable.cc \
	src/util/printable.hh \
	src/util/singleton.hh \
	src/util/string_split.hh \
	src/util/util.cc \
	src/util/util.hh \
	src/util/uuid.hh \
	src/util/db/connection.hh \
	src/util/db/connection_factory.hh \
	src/util/db/database.hh \
	src/util/db/db_exceptions.hh \
	src/util/db/manager.hh \
	src/util/db/manager_tss.hh \
	src/util/db/nullable.hh \
	src/util/db/param_query_composition.cc \
	src/util/db/param_query_composition.hh \
	src/util/db/query_param.hh \
	src/util/db/result.hh \
	src/util/db/row.hh \
	src/util/db/sequence.hh \
	src/util/db/statement.hh \
	src/util/db/transaction.hh \
	src/util/db/value.cc \
	src/util/db/value.hh \
	src/util/db/psql/psql_connection.hh \
	src/util/db/psql/psql_result.hh \
	src/util/log/context.cc \
	src/util/log/context.hh \
	src/util/log/log.cc \
	src/util/log/log.hh \
	src/util/log/log_types.cc \
	src/util/log/log_types.hh \
	src/util/log/logger.cc \
	src/util/log/logger.hh \
	src/util/types/common.hh \
	src/util/types/convert_common.hh \
	src/util/types/convert_sql_base.hh \
	src/util/types/convert_sql_boost_datetime.hh \
	src/util/types/convert_sql_db_types.hh \
	src/util/types/convert_sql_pod.hh \
	src/util/types/data_types.hh \
	src/util/types/date.cc \
	src/util/types/date.hh \
	src/util/types/date_interval.cc \
	src/util/types/date_interval.hh \
	src/util/types/datetime.cc \
	src/util/types/datetime.hh \
	src/util/types/datetime_common.hh \
	src/util/types/datetime_interval.cc \
	src/util/types/datetime_interval.hh \
	src/util/types/id.cc \
	src/util/types/id.hh \
	src/util/types/null.hh \
	src/util/types/sqlize.hh

libfred_la_LDFLAGS = -version-info 0:0:0

libfred_la_LIBADD = \
		$(BOOST_DATE_TIME_LIB) \
		$(BOOST_REGEX_LIB) \
		$(BOOST_THREAD_LIB) \
		$(BOOST_SYSTEM_LIB) \
		$(POSTGRESQL_LIB)
		