# Sources
set(SRC
    src/synthesizer_vpcc.cpp
    src/callback_vpcc.cpp    
    src/VPCCRenderer.cpp
    src/VPCCModelBuilder.cpp

    include/types_vpcc.h
    include/synthesizer_vpcc.h
    include/VPCCRenderer.h
    include/VPCCModelBuilder.h
)

INCLUDE_DIRECTORIES( include
					 ${TMIV_INC_DIR}
					 ${TMC2_DIR}/source/lib/PccLibBitstreamCommon/include)

# Libraries
add_library(V3CImmersiveSynthesizerVPCC SHARED ${SRC})
target_include_directories(V3CImmersiveSynthesizerVPCC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

##iloj
target_link_libraries(V3CImmersiveSynthesizerVPCC
PRIVATE
iloj::iloj
)

## Common
target_include_directories(V3CImmersiveSynthesizerVPCC
PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../common/include
)
target_link_libraries(V3CImmersiveSynthesizerVPCC
PRIVATE
V3CImmersiveCommon
)
if(ANDROID)
	target_link_directories(V3CImmersiveSynthesizerVPCC
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/android/arm64-v8a/Sources/common
	)
else()
	target_link_directories(V3CImmersiveSynthesizerVPCC
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/msvc/${INSTALL_MODE}/Sources/common/${MODE}
	)
endif()

# Install
if(ANDROID)
	install(TARGETS V3CImmersiveSynthesizerVPCC LIBRARY DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/${CMAKE_SYSTEM_NAME}/${BUILD_MODE}/${ANDROID_ABI}/")
else()
	install(TARGETS V3CImmersiveSynthesizerVPCC RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/${CMAKE_SYSTEM_NAME}/${BUILD_MODE}/x86_64/")
endif()