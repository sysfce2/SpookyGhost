if(NOT DEFINED PACKAGE_NAME)
	message(FATAL_ERROR "PACKAGE_NAME variable is not set")
endif()

if(NOT DEFINED PACKAGE_SOURCES)
	message(FATAL_ERROR "PACKAGE_SOURCES variable is not set")
endif()

if(NOT DEFINED PACKAGE_REVERSE_DNS)
	message(WARNING "PACKAGE_REVERSE_DNS variable is not set")
	set(PACKAGE_DESKTOP_FILE ${PACKAGE_LOWER_NAME}.desktop)
	set(PACKAGE_JAVA_URL ${PACKAGE_LOWER_NAME})
else()
	string(FIND ${PACKAGE_REVERSE_DNS} "-" DASH_FOUND_IN_DNS)
	if(DASH_FOUND_IN_DNS GREATER -1)
		message(WARNING "PACKAGE_REVERSE_DNS variable cannot contain a dash character")
		string(REPLACE "-" "_" PACKAGE_REVERSE_DNS ${PACKAGE_REVERSE_DNS})
	endif()

	set(PACKAGE_DESKTOP_FILE ${PACKAGE_REVERSE_DNS}.desktop)
	string(REPLACE "." "/" PACKAGE_JAVA_URL ${PACKAGE_REVERSE_DNS})
endif()

if(PACKAGE_BUILD_ANDROID)
	if(NOT DEFINED PACKAGE_ANDROID_ASSETS)
		message(WARNING "PACKAGE_ANDROID_ASSETS variable is not set")
	elseif(NOT PACKAGE_ANDROID_ASSETS)
		message(WARNING "PACKAGE_ANDROID_ASSETS variable is set but it is an empty list")
	endif()
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
	message(WARNING "Consider adding a \"LICENSE\" file in the root of the project")
endif()

string(TOLOWER ${PACKAGE_NAME} PACKAGE_LOWER_NAME)
string(TOUPPER ${PACKAGE_NAME} PACKAGE_UPPER_NAME)

if(NOT DEFINED PACKAGE_EXE_NAME)
	set(PACKAGE_EXE_NAME ${PACKAGE_LOWER_NAME})
endif()

if(NOT DEFINED PACKAGE_DESKTOP_FILE)
	set(PACKAGE_DESKTOP_FILE ${PACKAGE_LOWER_NAME}.desktop)
endif()

if(NOT DEFINED PACKAGE_JAVA_URL)
	set(PACKAGE_JAVA_URL ${PACKAGE_LOWER_NAME})
endif()

message("${PACKAGE_NAME} by ${PACKAGE_VENDOR}")
message("${PACKAGE_DESCRIPTION}")
message("${PACKAGE_HOMEPAGE}")
message("${PACKAGE_COPYRIGHT}\n")
