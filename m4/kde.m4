AC_DEFUN([KDE_CONFIG_OPTIONS],
[
	AC_ARG_WITH(
		[kde4-config],
		[AS_HELP_STRING([--with-kde4-config=PATH],
			[kde4-config script to use])],
		[KDE4_CONFIG=$withval], [KDE4_CONFIG=""])
	AC_ARG_ENABLE(
		[kde-in-home],
		[AS_HELP_STRING([--enable-kde-in-home],
			[install KDE related stuff in your home dir])],
		[KDE_IN_HOME=$enableval], [KDE_IN_HOME=no])
])

AC_DEFUN([KDE_CONFIG_CHECK],
[
	AS_IF([test -z ${KDE4_CONFIG}],
	[
		AC_PATH_PROG(KDE4_CONFIG, kde4-config)
	],
	[
		AC_MSG_CHECKING(for kde4-config)
		AS_IF([test ! -x KDE4_CONFIG],
		[
			KDE4_CONFIG="not found"
			AC_MSG_RESULT(not found)
		])
	])

	AS_IF([ test "${KDE4_CONFIG}" != "not found"],
	[
		KDE_CONF_VER=`${KDE4_CONFIG} --version | grep KDE | sed -e 's/^.*: //' -e 's/ (.*$//'`
		AS_IF([test `echo ${KDE_CONF_VER} | sed -e 's/\..*$//'` = 4],
		[
			AC_MSG_RESULT(found ${KDE4_CONFIG} with version ${KDE_CONF_VER})
		],
		[
			AC_MSG_RESULT(not found)
		])
	],
	[
		AC_MSG_RESULT(not found)
	])
	AC_SUBST(KDE4_CONFIG)
])

AC_DEFUN([KDE_HEADER_CHECK],
[
	AC_MSG_CHECKING(for kde4 headers)
	[KDE_HEADER_DIR=`${KDE4_CONFIG} --path include`]

	AS_IF([test -f ${KDE_HEADER_DIR+kdirwatch.h}],
	[
		AS_IF([test -f ${KDE_HEADER_DIR+plasma/dataengine.h}],
		[
			AC_MSG_RESULT($KDE_HEADER_DIR)
		],
		[
			AC_MSG_RESULT(not found)
		])
	],
	[
		AC_MSG_RESULT(not found)
	])
	AC_SUBST(KDE_HEADER_DIR)
])

AC_DEFUN([KDE_SERVICE_PATH_CHECK],
[
	AC_MSG_CHECKING(for kde4 services Path)
	AS_IF([test `echo ${prefix} | grep home | wc -l` == 1],
	[
		KDE_IN_HOME="yes"
	])
	AS_IF([test ${KDE_IN_HOME} = "yes"],
	[
		KDE_SERVICE_PATH=`${KDE4_CONFIG} --path services | tr ":" "\n" | grep /home | head -1`
	],
	[
		KDE_SERVICE_PATH=`${KDE4_CONFIG} --path services | tr ":" "\n" | grep /usr | head -1`
	])
	AC_MSG_RESULT(${KDE_SERVICE_PATH})
	AC_SUBST(KDE_SERVICE_PATH)
])

AC_DEFUN([KDE_MODULE_PATH_CHECK],
[
	AC_MSG_CHECKING(for kde4 plugins Path)
	AS_IF([test `echo ${prefix} | grep home | wc -l` == 1],
	[
		KDE_IN_HOME="yes"
	])
	AS_IF([test ${KDE_IN_HOME} = "yes"],
	[
		KDE_MODULE_PATH=`${KDE4_CONFIG} --path module | tr ":" "\n" | grep /home | head -1`
	],
	[
		KDE_MODULE_PATH=`${KDE4_CONFIG} --path module | tr ":" "\n" | grep /usr | head -1`
	])
	AC_MSG_RESULT(${KDE_MODULE_PATH})
	AC_SUBST(KDE_MODULE_PATH)
])

AC_DEFUN([KDE_ICON_PATH_CHECK],
[
	AC_MSG_CHECKING(for kde4 icons Path)
	AS_IF([test `echo ${prefix} | grep home | wc -l` == 1],
	[
		KDE_IN_HOME="yes"
	])
	AS_IF([test ${KDE_IN_HOME} = "yes"],
	[
		KDE_ICON_PATH=`${KDE4_CONFIG} --path icon | tr ":" "\n" | grep /home | head -1`
	],
	[
		KDE_ICON_PATH=`${KDE4_CONFIG} --path icon | tr ":" "\n" | grep /usr | head -1`
	])
	AC_MSG_RESULT(${KDE_ICON_PATH})
	AC_SUBST(KDE_ICON_PATH)
])
