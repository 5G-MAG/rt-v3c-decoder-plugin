#!/bin/bash

trap exit ERR

if [ $# -eq 0 ]
  then
	echo "ERROR: script needs input arguments, options = [debug|release] [all|tmiv|tmc2|zlib|glfw|dash]"; exit 1;
else 
	for i in "$@"; do
	  case "$i" in
		debug|Debug|DEBUG         ) MODE=Debug;   INSTALL_MODE=debug;;
		release|Release|RELEASE   ) MODE=Release; INSTALL_MODE=release;;
		zlib|Zlib|ZLIB            ) COMPIL_ZLIB=ON;;
		glfw|GLfw|GLFW            ) COMPIL_GLFW=ON;;
		tmiv|Tmiv|TMIV            ) COMPIL_TMIV=ON;;
		tmc2|Tmc2|TMC2            ) COMPIL_TMC2=ON;;
		dash|Dash|DASH            ) COMPIL_DASH=ON;;
		mhrm|Mhrm|MHRM            ) COMPIL_MHRM=ON;;
		proj			          ) COMPIL_PROJECT=ON;;
		all|All|ALL               ) COMPIL_ZLIB=ON; COMPIL_GLFW=ON; COMPIL_TMIV=ON; COMPIL_TMC2=ON; COMPIL_DASH=ON; COMPIL_MHRM=ON; COMPIL_PROJECT=ON;;
		clean|Clean|CLEAN         ) MODE=Clean;;
		*                         ) echo "ERROR: arguments \"$i\" not supported: option = [debug|release] [all|tmiv|tmc2|zlib|glfw|dash] "; exit 1;;
	  esac
	done
fi

PROJECT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/..

AVCODEC_VER=7.1
AVCODEC_DIR=${PROJECT_DIR}/External/avcodec/${AVCODEC_VER}/Windows/x86_64

ZLIB_VER=1.3.1
ZLIB_DIR=${PROJECT_DIR}/External/zlib/${ZLIB_VER}

GLFW_VER=3.4
GLFW_DIR=${PROJECT_DIR}/External/glfw/${GLFW_VER}

ILOJ_LIBS=${PROJECT_DIR}/Libraries/iloj/${MODE}/msvc

TMIV_VER=23.0
TMIV_DIR=${PROJECT_DIR}/External/tmiv/${TMIV_VER}

TMC2_VER=25.0
TMC2_DIR=${PROJECT_DIR}/External/mpeg-pcc-tmc2/${TMC2_VER}

MHRM_VER=5.1
MHRM_DIR=${PROJECT_DIR}/External/mpeg-haptics-rm/${MHRM_VER}

### check dependencies
if [ ! -d "${TMIV_DIR}" ] ; then echo "run dl_deps.sh to install tmiv dependencies" ; exit; fi
if [ ! -d "${TMC2_DIR}" ] ; then echo "run dl_deps.sh to install tmc2 dependencies" ; exit; fi
if [ ! -d "${MHRM_DIR}" ] ; then echo "run dl_deps.sh to install mhrm dependencies" ; exit; fi
if [ ! -d "${AVCODEC_DIR}" ] ; then echo "${AVCODEC_DIR} not found" ; exit; fi

### find Visual Studio latest version
VS_EXE=/C/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio/Installer/vswhere.exe
A=`"$VS_EXE" -nologo -latest -property catalog_productName`
B=`"$VS_EXE" -nologo -latest -property catalog_productDisplayVersion | awk 'BEGIN {FS="."} {print $1}'`
C=`"$VS_EXE" -nologo -latest -property catalog_productLineVersion`
MS_GENERATOR="$A $B $C"
echo -e "\033[0;32mVisual Studio found: ${MS_GENERATOR} \033[0m";

### clean mode
if [ "$MODE" == "Clean" ]; then
	echo -e "\033[0;32mClean: ${ZLIB_DIR} \033[0m";	    rm -rf ${ZLIB_DIR}/build/msvc;
	echo -e "\033[0;32mClean: ${GLFW_DIR} \033[0m";	    rm -rf ${GLFW_DIR}/build/msvc;
	echo -e "\033[0;32mClean: ${TMIV_DIR} \033[0m";	    rm -rf ${TMIV_DIR}/build/msvc;
	echo -e "\033[0;32mClean: ${TMC2_DIR} \033[0m";	    rm -rf ${TMC2_DIR}/build/msvc;
	echo -e "\033[0;32mClean: ${MHRM_DIR} \033[0m";	    rm -rf ${MHRM_DIR}/build/msvc;
	echo -e "\033[0;32mClean: ${PROJECT_DIR} \033[0m";	rm -rf ${PROJECT_DIR}/build/msvc;
	echo -e "\033[0;32mClean done.\033[0m";
	exit 0;
fi

mkdir -p ${PROJECT_DIR}/Output/Windows/${MODE}/x86_64
cp 	${AVCODEC_DIR}/bin/*.dll \
${PROJECT_DIR}/Output/Windows/${MODE}/x86_64

### zlib
if [[ "$COMPIL_ZLIB" == "ON" ]]; then
	echo -e "\033[0;32mBuild: ${ZLIB_DIR} \033[0m";
	mkdir -p ${ZLIB_DIR}/build/msvc/${INSTALL_MODE}
	cd ${ZLIB_DIR}/build/msvc/${INSTALL_MODE}
	rm -f CMakeLists.txt
	cmake \
		-G"$MS_GENERATOR" \
		-DCMAKE_INSTALL_PREFIX=${ZLIB_DIR}/Windows/x86_64 \
		${ZLIB_DIR}
	cmake --build . --config ${MODE} --target install --parallel
fi

### glfw
if [[ "$COMPIL_GLFW" == "ON" ]]; then
	echo -e "\033[0;32mBuild: ${GLFW_DIR} \033[0m";
	mkdir -p ${GLFW_DIR}/build/msvc/${INSTALL_MODE}
	cd ${GLFW_DIR}/build/msvc/${INSTALL_MODE}
	rm -f CMakeLists.txt
	cmake \
		-G"$MS_GENERATOR" \
		-DCMAKE_INSTALL_PREFIX=${GLFW_DIR}/Windows/x86_64 \
		${GLFW_DIR}
	cmake --build . --config ${MODE} --target install --parallel
fi

### tmiv
if [[ "$COMPIL_TMIV" == "ON" ]]; then
	echo -e "\033[0;32mBuild: ${TMIV_DIR} \033[0m";
	mkdir -p ${TMIV_DIR}/build/msvc/${INSTALL_MODE}
	cd ${TMIV_DIR}/build/msvc/${INSTALL_MODE}
	rm -f CMakeLists.txt
	cmake \
		-G"$MS_GENERATOR" \
		-DCMAKE_INSTALL_PREFIX=${TMIV_DIR}/${INSTALL_MODE}/msvc \
		-DBUILD_TESTING=OFF \
		-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON \
		-DBUILD_SHARED_LIBS=ON \
		${TMIV_DIR}
	cmake --build . --config ${MODE} --target install --parallel

	cp 	${TMIV_DIR}/${INSTALL_MODE}/msvc/bin/CommonLib.dll \
		${TMIV_DIR}/${INSTALL_MODE}/msvc/bin/DecoderLib.dll \
		${TMIV_DIR}/${INSTALL_MODE}/msvc/bin/MivBitstreamLib.dll \
		${TMIV_DIR}/${INSTALL_MODE}/msvc/bin/ViewingSpaceLib.dll \
	${PROJECT_DIR}/Output/Windows/${MODE}/x86_64
fi

### tmc2
if [[ "$COMPIL_TMC2" == "ON" ]]; then
	echo -e "\033[0;32mBuild: ${TMC2_DIR} \033[0m";
	mkdir -p ${TMC2_DIR}/build/msvc/${INSTALL_MODE}
	cd ${TMC2_DIR}/build/msvc/${INSTALL_MODE}
	cmake \
		-G"$MS_GENERATOR" \
		-DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
		-DLINKER=SHARED \
		-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON \
		-DUSE_JMAPP_VIDEO_CODEC=FALSE \
		-DUSE_HMAPP_VIDEO_CODEC=TRUE \
		-DUSE_SHMAPP_VIDEO_CODEC=FALSE \
		-DUSE_JMLIB_VIDEO_CODEC=FALSE \
		-DUSE_HMLIB_VIDEO_CODEC=TRUE \
		-DUSE_VTMLIB_VIDEO_CODEC=FALSE \
		-DUSE_VVLIB_VIDEO_CODEC=FALSE \
		-DUSE_FFMPEG_VIDEO_CODEC=FALSE \
		-DENABLE_TBB=FALSE \
		-DCONFORMANCE_TRACE=FALSE \
		${TMC2_DIR}
	cmake --build . --config ${MODE} --parallel
	
	cp 	${TMC2_DIR}/bin/${MODE}/PccLibBitstreamCommon.dll \
		${TMC2_DIR}/bin/${MODE}/PccLibBitstreamReader.dll \
		${TMC2_DIR}/bin/${MODE}/PccLibColorConverter.dll \
		${TMC2_DIR}/bin/${MODE}/PccLibCommon.dll \
		${TMC2_DIR}/bin/${MODE}/PccLibDecoder.dll \
		${TMC2_DIR}/bin/${MODE}/PccLibVideoDecoder.dll \
		${TMC2_DIR}/bin/${MODE}/PccLibHevcParser.dll \
	${PROJECT_DIR}/Output/Windows/${MODE}/x86_64
fi

### mpeg haptics rm
if [[ "$COMPIL_MHRM" == "ON" ]]; then
	echo -e "\033[0;32mBuild: ${MHRM_DIR} \033[0m";
	mkdir -p ${MHRM_DIR}/build/msvc/${INSTALL_MODE}
	cd ${MHRM_DIR}/build/msvc/${INSTALL_MODE}
	cmake \
		-G"$MS_GENERATOR" \
		-DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
		-DENABLE_CLANG_TIDY=FALSE \
		-DBUILD_DECODER=OFF \
		-DBUILD_ENCODER=OFF \
		-DBUILD_SYNTHESIZER=OFF \
		${MHRM_DIR}
	cmake --build . --config ${MODE} --parallel
fi

### project
if [[ "$COMPIL_PROJECT" == "ON" ]]; then
	echo -e "\033[0;32mBuild: ${PROJECT_DIR} \033[0m";
	cd ${PROJECT_DIR}
	mkdir -p ${PROJECT_DIR}/build/msvc/${INSTALL_MODE}
	cd ${PROJECT_DIR}/build/msvc/${INSTALL_MODE}
	rm -f CMakeLists.txt
	cmake \
		-G"$MS_GENERATOR" \
		-Diloj_DIR=${ILOJ_LIBS}/lib/cmake \
		-DILOJ_INC_DIR=${ILOJ_LIBS}/include \
		-DTMIV_DIR=${TMIV_DIR}/${INSTALL_MODE}/msvc/lib/cmake/TMIV \
		-DTMIV_INC_DIR=${TMIV_DIR}/${INSTALL_MODE}/msvc/include \
		-DTMIV_LIB_DIR=${TMIV_DIR}/${INSTALL_MODE}/msvc/lib \
		-DTMC2_DIR=${TMC2_DIR} \
		-DTMC2_LIB_DIR=${TMC2_DIR}/lib/${MODE} \
		-DMHRM_DIR=${MHRM_DIR} \
		-DMHRM_LIB_DIR=${MHRM_DIR}/build/msvc/${INSTALL_MODE}/source \
		-DMHRM_LIB_DEPS=${MHRM_DIR}/build/msvc/${INSTALL_MODE}/_deps \
		-DBUILD_MODE=${MODE} \
        -DUSE_DASH=${COMPIL_DASH} \
		-DUSE_STREAMING_LOG="false" \
		${PROJECT_DIR}
	cmake --build . --config ${MODE} --target install --parallel

	cd ${PROJECT_DIR}/Scripts
fi
