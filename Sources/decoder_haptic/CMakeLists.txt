# Sources
set(SRC
    src/haptic.cpp
    src/decoder_haptic.cpp
	
    include/types.h
    include/decoder_haptic.h
)

INCLUDE_DIRECTORIES( include
					 ${TMIV_INC_DIR})

# Libraries
add_library(V3CImmersiveDecoderHaptic SHARED ${SRC})
target_include_directories(V3CImmersiveDecoderHaptic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

##iloj
target_link_libraries(V3CImmersiveDecoderHaptic
PRIVATE
iloj::iloj
)

## tmiv
target_link_libraries(V3CImmersiveDecoderHaptic
PRIVATE
TMIV::DecoderLib
)

## Common
target_include_directories(V3CImmersiveDecoderHaptic
PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)
target_link_libraries(V3CImmersiveDecoderHaptic
PRIVATE
V3CImmersiveCommon
)
if(ANDROID)
	target_link_directories(V3CImmersiveDecoderHaptic
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../build/android/arm64-v8a/Sources/common
	)
else()
	target_link_directories(V3CImmersiveDecoderHaptic
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../build/msvc/${INSTALL_MODE}/Sources/common/${BUILD_MODE}
	)
endif()

##MHRM
include_directories("${MHRM_DIR}/source")
include_directories("${MHRM_LIB_DEPS}/rapidjson-src/include")
link_directories("${MHRM_LIB_DIR}/")
if(ANDROID)
    file(GLOB MHRMLibs 
		"${MHRM_LIB_DIR}/Types/*.a"
		"${MHRM_LIB_DIR}/Tools/*.a"
		"${MHRM_LIB_DIR}/IOHaptics/*.a")
else()
    file(GLOB MHRMLibs 
		"${MHRM_LIB_DIR}/Types/${BUILD_MODE}/*.lib"
		"${MHRM_LIB_DIR}/Tools/${BUILD_MODE}/*.lib"
		"${MHRM_LIB_DIR}/IOHaptics/${BUILD_MODE}/*.lib")
endif()
target_link_libraries(V3CImmersiveDecoderHaptic
PUBLIC
${MHRMLibs}
)

# Install
if(ANDROID)
	install(TARGETS V3CImmersiveDecoderHaptic LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/${CMAKE_SYSTEM_NAME}/${BUILD_MODE}/${ANDROID_ABI}/")
else()
	install(TARGETS V3CImmersiveDecoderHaptic RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/${CMAKE_SYSTEM_NAME}/${BUILD_MODE}/x86_64/")
endif()