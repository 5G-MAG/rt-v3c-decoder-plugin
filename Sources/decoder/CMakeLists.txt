# Targets
add_library(V3CImmersiveDecoderVideo SHARED "")
add_library(AudioPlugin_V3CImmersiveDecoderAudio SHARED "")
set (H_main 
	include/main/interface.h)


set(H_interface 
	include/interface/audio.h
	include/interface/client.h
	include/interface/decoder.h
	include/interface/scheduler.h
	include/interface/video.h
)

set(H_components 
	include/client/reader.h
	include/client/meta.h
	include/decoder/decoder.h
	include/scheduler/scheduler.h
	include/audio/buffer.h
	include/audio/audio.h
	include/video/video.h
	

)

set(C_main 
	src/main/core.cpp
	src/main/interface.cpp)

set (C_components
	src/client/reader.cpp
	src/client/meta.cpp
	src/decoder/decoder.cpp
	src/scheduler/scheduler.cpp
	src/audio/audio.cpp
	src/video/video.cpp
)

target_sources(AudioPlugin_V3CImmersiveDecoderAudio
PRIVATE
src/audio/buffer.cpp
src/unity/audio.cpp
include/audio/buffer.h
)

message(STATUS "Checking complementary modules")

message(STATUS "Adding haptic sources")
set(H_haptic_interface include/interface/haptic.h)

set(H_haptic 
	include/decoder/decoder_haptic.h 
	include/haptic/haptic.h)

set(C_haptic 
	src/decoder/decoder_haptic.cpp
	src/haptic/haptic.cpp)


message(STATUS "Adding streaming sources")

if(USE_DASH)
	message(STATUS "Adding streaming sources for DASH")
	set(H_streaming_dash include/streaming/client_dash.h)
	set(C_streaming_dash src/streaming/client_dash.cpp)
else()
	set(H_streaming_dash )
	set(C_streaming_dash )
endif()

if(USE_UVG_RTP)
	message(STATUS "Adding streaming sources for UVG RTP")
	set(H_streaming_uvg include/streaming/receiver_uvg_rtp.h)
	set(C_streaming_uvg src/streaming/receiver_uvg_rtp.cpp)
else()
	set(H_streaming_uvg )
	set(C_streaming_uvg )
endif()

if(USE_WEBRTC_RTP)
	message(STATUS "Adding streaming sources for WEBRTC RTP")
	set(H_streaming_webrtc include/streaming/receiver_webrtc.h)
	set(C_streaming_webrtc src/streaming/receiver_webrtc.cpp)
endif()

set(H_streaming_components 
	include/streaming/network_interface.h
	${H_streaming_dash}
	${H_streaming_uvg}
	${H_streaming_webrtc}
)

set(C_streaming_components 
	src/streaming/network_interface.cpp
	${C_streaming_dash}
	${C_streaming_uvg}
	${C_streaming_webrtc}
)

source_group("Source Files\\main"		FILES ${C_main})
source_group("Source Files\\components"	FILES ${C_components} ${C_haptic} ${C_streaming_components})
source_group("Header Files\\interface"	FILES ${H_interface} ${H_haptic_interface})
source_group("Header Files\\main"		FILES ${H_main})
source_group("Header Files\\components" FILES ${H_components} ${H_streaming_components} ${H_haptic})



# Sources
target_sources(V3CImmersiveDecoderVideo
PRIVATE
${H_interface}
${H_main}
${H_components}
${H_haptic_interface}
${H_haptic}
${H_streaming_components}
${C_main}
${C_components}
${C_haptic}
${C_streaming_components}
)

target_include_directories(V3CImmersiveDecoderVideo
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(AudioPlugin_V3CImmersiveDecoderAudio
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Dependencies
## Common
target_link_libraries(V3CImmersiveDecoderVideo
PRIVATE
V3CImmersiveCommon
)
## OpenGL
if(ANDROID)
	set(OPENGL_LIBRARIES EGL GLESv3)
else()
	find_package(OpenGL REQUIRED)
endif()
target_link_libraries(V3CImmersiveDecoderVideo PRIVATE ${OPENGL_LIBRARIES})

##iloj
include_directories("${ILOJ_INC_DIR}")
target_link_libraries(AudioPlugin_V3CImmersiveDecoderAudio
PRIVATE
iloj::iloj
)
target_link_libraries(V3CImmersiveDecoderVideo
PRIVATE
iloj::iloj
)

##TMC2
include_directories("${TMC2_DIR}/source/lib/PccLibBitstreamCommon/include")
link_directories("${TMC2_LIB_DIR}/")
target_link_libraries(V3CImmersiveDecoderVideo
PRIVATE
TMIV::ViewingSpaceLib
)

# Install
if(ANDROID)
	set(plugins_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../Output/${CMAKE_SYSTEM_NAME}/${BUILD_MODE}/${ANDROID_ABI})
	
	install(TARGETS V3CImmersiveDecoderVideo LIBRARY DESTINATION ${plugins_dir})
	install(TARGETS AudioPlugin_V3CImmersiveDecoderAudio LIBRARY DESTINATION ${plugins_dir})
	#get_property(JARNAME TARGET iloj::ilojj PROPERTY IMPORTED_LOCATION)
	#install(FILES ${JARNAME} DESTINATION ${plugins_dir}/../.. RENAME "V3CImmersiveDecoderVideo.jar")

else()
	set(plugins_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../Output/${CMAKE_SYSTEM_NAME}/${BUILD_MODE}/x86_64)
	
	install(TARGETS V3CImmersiveDecoderVideo RUNTIME DESTINATION ${plugins_dir})
	install(TARGETS AudioPlugin_V3CImmersiveDecoderAudio RUNTIME DESTINATION ${plugins_dir})
	
endif()
