function(portce_add_web_shell target)
	if(NOT TARGET "${target}")
		message(FATAL_ERROR "portce_add_web_shell: target '${target}' does not exist.")
	endif()

	set(options)
	set(one_value_args OUT_DIR TEMPLATE TITLE)
	set(multi_value_args)
	cmake_parse_arguments(PORTCE_WEB "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	if(NOT PORTCE_WEB_OUT_DIR)
		message(FATAL_ERROR "portce_add_web_shell: OUT_DIR is required.")
	endif()

	if(NOT PORTCE_WEB_TEMPLATE)
		message(FATAL_ERROR "portce_add_web_shell: TEMPLATE is required.")
	endif()

	get_filename_component(PORTCE_WEB_OUT_DIR_ABS "${PORTCE_WEB_OUT_DIR}" ABSOLUTE)
	file(MAKE_DIRECTORY "${PORTCE_WEB_OUT_DIR_ABS}")

	set(PORTCE_APP_JS "${target}.js")
	if(PORTCE_WEB_TITLE)
		set(PORTCE_WEB_TITLE_TEXT "${PORTCE_WEB_TITLE}")
	else()
		set(PORTCE_WEB_TITLE_TEXT "PortCE Web")
	endif()

	configure_file(
		"${PORTCE_WEB_TEMPLATE}"
		"${PORTCE_WEB_OUT_DIR_ABS}/${target}.html"
		@ONLY
	)
endfunction()
