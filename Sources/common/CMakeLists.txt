# Sources
set(SRC
    "src/stream/item.cpp"
    "src/decoder/miv.cpp"
    "src/decoder/vpcc.cpp"
    "src/video/pose.cpp"
    "src/video/job.cpp"
    "src/video/texture.cpp"


    "include/common/stream/chunk.h"
    "include/common/stream/item.h"
    "include/common/decoder/miv.h"
    "include/common/decoder/vpcc.h"
    "include/common/video/pose.h"
    "include/common/video/job.h"
    "include/common/video/texture.h"
    "include/common/misc/types.h"
    "include/common/misc/types_haptic.h"
	"include/common/misc/spsc_queue.h"
    "include/common/texture_format.h"
)

include_directories(
	include
    "${ILOJ_INC_DIR}"
    "${TMIV_INC_DIR}"
    "${TMC2_DIR}/source/lib/PccLibBitstreamCommon/include"
    "${TMC2_DIR}/source/lib/PccLibBitstreamReader/include"
    "${TMC2_DIR}/source/lib/PccLibCommon/include "
    "${TMC2_DIR}/source/lib/PccLibDecoder/include"
    "${TMC2_DIR}/dependencies/nanoflann"
    "${TMC2_DIR}/dependencies/tbb/include"
    "${TMC2_DIR}/dependencies/libmd5"
)


# Libraries

# Fetch all TMIV libs
link_directories("${TMIV_LIB_DIR}/")
if(ANDROID)
    file(GLOB MIVLibs "${TMIV_LIB_DIR}/*.a" "${TMIV_LIB_DIR}/*.so")
else()
    file(GLOB MIVLibs "${TMIV_LIB_DIR}/*.lib")
endif()

# Fetch all TMC2 libs
link_directories("${TMC2_LIB_DIR}/")
if(ANDROID)
    file(GLOB PCCLibs "${TMC2_LIB_DIR}/*.a" "${TMC2_LIB_DIR}/*.so")
else()
    file(GLOB PCCLibs "${TMC2_LIB_DIR}/*.lib")
endif()

add_library(V3CImmersiveCommon STATIC ${SRC})

if(ANDROID)
	target_link_libraries(V3CImmersiveCommon
		PUBLIC
			fmt::fmt
	)
endif()

target_link_libraries(V3CImmersiveCommon
	PRIVATE
		iloj::iloj
		${MIVLibs}
		${PCCLibs}
)

target_include_directories(V3CImmersiveCommon PUBLIC include)

