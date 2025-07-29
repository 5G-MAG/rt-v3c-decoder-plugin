# Sources
set(SRC
    src/synthesizer.cpp
    src/haptic.cpp

    include/types.h
    include/synthesizer.h
)

INCLUDE_DIRECTORIES( include
					 ${TMIV_INC_DIR})

# Libraries
add_library(V3CImmersiveSynthesizerHaptic SHARED ${SRC})
target_include_directories(V3CImmersiveSynthesizerHaptic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Dependencies
##iloj
target_link_libraries(V3CImmersiveSynthesizerHaptic
PRIVATE
iloj::iloj
)

## Common
target_include_directories(V3CImmersiveSynthesizerHaptic
PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../common/include
)
target_link_libraries(V3CImmersiveSynthesizerHaptic
PRIVATE
V3CImmersiveCommon
)
if(ANDROID)
	target_link_directories(V3CImmersiveSynthesizerHaptic
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/android/arm64-v8a/Sources/common
	)
else()
	target_link_directories(V3CImmersiveSynthesizerHaptic
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/msvc/${INSTALL_MODE}/Sources/common/${BUILD_MODE}
	)
endif()

# Install
if(ANDROID)
	install(TARGETS V3CImmersiveSynthesizerHaptic LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/${CMAKE_SYSTEM_NAME}/${BUILD_MODE}/${ANDROID_ABI}/")
else()
	install(TARGETS V3CImmersiveSynthesizerHaptic RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/${CMAKE_SYSTEM_NAME}/${BUILD_MODE}/x86_64/")
endif()
