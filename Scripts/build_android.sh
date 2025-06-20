#!/bin/bash

trap exit ERR

if [ $# -eq 0 ]
  then
	echo "ERROR: script needs input arguments, options = [debug|release] [all|tmiv|tmc2|mhrm|dash]"; exit 1;
else 
	for i in "$@"; do
	  case "$i" in
		debug|Debug|DEBUG         ) MODE=Debug;   INSTALL_MODE=debug;;
		release|Release|RELEASE   ) MODE=Release; INSTALL_MODE=release;;
		tmiv|Tmiv|TMIV            ) COMPIL_FMT=ON; COMPIL_TMIV=ON;;
		tmc2|Tmc2|TMC2            ) COMPIL_TMC2=ON;;
		dash|Dash|DASH            ) COMPIL_DASH=ON;;
		mhrm|Mhrm|MHRM            ) COMPIL_MHRM=ON;;
		all|All|ALL               ) COMPIL_FMT=ON; COMPIL_TMIV=ON; COMPIL_TMC2=ON; COMPIL_DASH=ON; COMPIL_MHRM=ON; COMPIL_PROJECT=ON;;
		clean|Clean|CLEAN         ) MODE=Clean;;
		*                         ) echo "ERROR: arguments \"$i\" not supported: option = [debug|release] [all|tmiv|tmc2|mhrm|dash] "; exit 1;;
	  esac
	done
fi

ABI=arm64-v8a
USERNAME=$(whoami)
USER=${USERNAME##*+}

### retrieve Android SDK and NDK path
if [ -f "./user_ndk_path" ]
then 
	echo "Using saved configuration to retrieve Android SDK and NDK path."
	ANDROID_SDK="/c/Users/$USER/AppData/Local/Android/Sdk"
	ANDROID_NDK=$(head -n 1 "user_ndk_path")
else
		
	BUILD_PATH="$PWD"
	ANDROID_SDK="/c/Users/${USER}/AppData/Local/Android/Sdk"
	ANDROID_NDK="/c/Users/${USER}/AppData/Local/Android/Sdk/ndk"

	cd $ANDROID_NDK
	echo "Looking for Android NDKs in $PWD"
	echo ""

	# Enable special handling to prevent expansion to a
	# literal '/tmp/backup/*' when no matches are found. 
	shopt -s nullglob

	FOLDERS=("$ANDROID_NDK"/*/)
	num_folders=${#FOLDERS[@]}

	# Unset shell option after use, if desired. Nullglob
	# is unset by default.
	shopt -u nullglob

	valid_input=0
	while [ $valid_input = 0 ]
	do
		default_val=$(($num_folders - 1))
		echo "$num_folders results found, please select one [default: $default_val]:"
		for i in "${!FOLDERS[@]}"; do
			[[ -d "${FOLDERS[$i]}" ]] && echo "$i: ${FOLDERS[$i]}"
			# [[ -d "${FOLDERS[$i]}" ]]
			# printf "%s : %s\n" "$i" "$FOLDERS" 
		done

		read -i "$default_val" ndk_index 
		ndk_index="${ndk_index:-$default_val}"

		if [ $ndk_index -lt $num_folders ] && [ $ndk_index -gt -1 ]
		then
			ANDROID_NDK=${FOLDERS[$ndk_index]}
			echo "$ANDROID_NDK selected"
			valid_input=1
		else
			echo "Invalid Input"
		fi
	done

	cd $BUILD_PATH
	valid_input=0

	while [ $valid_input = 0 ]
	do
		read -p  "Save this path for later compilations? (Y/n)" input
		input="${input:-Y}"
		
		if [ $input = Y ]
		then 
			valid_input=1
			touch "./user_ndk_path"
			printf "%s" "$ANDROID_NDK">"./user_ndk_path"
		elif [ $input = n ]
		then
			valid_input=1
		else
			echo "Invalid Input"
		fi
	done 
fi


#--- Retrieve the latest API number ---#

PLATFORMS=$ANDROID_SDK/platforms
API_FOLDERS=("$PLATFORMS"/*/)
num_folders=${#API_FOLDERS[@]}
CANDIDATE_API_FOLDER=${API_FOLDERS[$((num_folders - 1))]}
CANDIDATE_API_FOLDER="${CANDIDATE_API_FOLDER%/}"
CANDIDATE_API_FOLDER="${CANDIDATE_API_FOLDER##*/}"	#keep the last part of the path

#split at the dash to isolate API number
SPLITTED=(${CANDIDATE_API_FOLDER//-/ })
API=${SPLITTED[1]}

#check that the folder exists
CHECK=$PLATFORMS/android-$API
echo "Check Android API "$CHECK
if [ ! -d $CHECK ]; then
	echo "Android API $API not detected"
	echo "Please install Android API "$API
	echo "error in API identification"; exit 1;
else
	echo "Android API $API detected"
fi


ANDROID_TOOLCHAIN_PATH="${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64"
PROJECT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/..

AVCODEC_VER=7.1
AVCODEC_DIR=${PROJECT_DIR}/External/avcodec/${AVCODEC_VER}/Android/${ABI}

ILOJ_LIBS=${PROJECT_DIR}/Libraries/iloj/${MODE}/android/${ABI}

FMT_VER=11.0.2
FMT_DIR=${PROJECT_DIR}/External/fmt/${FMT_VER}

TMIV_VER=23.0
TMIV_DIR=${PROJECT_DIR}/External/tmiv/${TMIV_VER}

TMC2_VER=25.0
TMC2_DIR=${PROJECT_DIR}/External/mpeg-pcc-tmc2/${TMC2_VER}

MHRM_VER=5.1
MHRM_DIR=${PROJECT_DIR}/External/mpeg-haptics-rm/${MHRM_VER}

NINJA_VER=1.11.1
NINJA_DIR=${PROJECT_DIR}/External/ninja/${NINJA_VER}
NINJA_EXE="${NINJA_DIR}/ninja.exe"

### check dependencies
echo -e "\033[0;32mCheck dependencies\033[0m";
if [ ! -d "${FMT_DIR}" ] ; then echo "run dl_deps.sh fmt to install dependencies" ; exit; fi
if [ ! -d "${TMIV_DIR}" ] ; then echo "run dl_deps.sh to install tmiv dependencies" ; exit; fi
if [ ! -d "${TMC2_DIR}" ] ; then echo "run dl_deps.sh to install tmc2 dependencies" ; exit; fi
if [ ! -d "${MHRM_DIR}" ] ; then echo "run dl_deps.sh to install mhrm dependencies" ; exit; fi
if [ ! -d "${NINJA_DIR}" ] ; then echo "run dl_deps.sh ninja to install dependencies" ; exit; fi
if [ ! -d "${AVCODEC_DIR}" ] ; then echo "${AVCODEC_DIR} not found" ; exit; fi

mkdir -p ${PROJECT_DIR}/Output/Android/${MODE}/${ABI}
cp 	${AVCODEC_DIR}/lib/*.so \
${PROJECT_DIR}/Output/Android/${MODE}/${ABI}

### clean mode
if [ "$MODE" == "Clean" ]; then
	echo -e "\033[0;32mClean: ${FMT_DIR} \033[0m";	    rm -rf ${FMT_DIR}/build/android;
	echo -e "\033[0;32mClean: ${TMIV_DIR} \033[0m";	    rm -rf ${TMIV_DIR}/build/android;
	echo -e "\033[0;32mClean: ${TMC2_DIR} \033[0m";	    rm -rf ${TMC2_DIR}/build/android;
    echo -e "\033[0;32mClean: ${MHRM_DIR} \033[0m";     rm -rf ${MHRM_DIR}/build/android;
	echo -e "\033[0;32mClean: ${PROJECT_DIR} \033[0m";	rm -rf ${PROJECT_DIR}/build/android;
	echo -e "\033[0;32mClean done.\033[0m";
	exit 0;
fi


### fmt
if [[ "$COMPIL_FMT" == "ON" || ! -d "${FMT_DIR}/build/android/${ABI}" ]]; then
	echo -e "\033[0;32mBuild: ${FMT_DIR} \033[0m";
	mkdir -p ${FMT_DIR}/build/android/${ABI}
	cd ${FMT_DIR}/build/android/${ABI}
	rm -f CMakeCache.txt
	cmake \
		-G"Ninja" \
		-DCMAKE_MAKE_PROGRAM=${NINJA_EXE} \
		-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
		-DCMAKE_INSTALL_PREFIX=${FMT_DIR}/${INSTALL_MODE}/android/${ABI} \
		-DCMAKE_BUILD_TYPE=${MODE} \
		-DANDROID_ABI=${ABI} \
		-DANDROID_NATIVE_API_LEVEL=${API} \
		-DCMAKE_CXX_STANDARD=20 \
		-DFMT_TEST=OFF \
		${FMT_DIR}
	cmake --build . --config ${MODE} --target install --parallel
fi

### tmiv
if [[ "$COMPIL_TMIV" == "ON" || ! -d "${TMIV_DIR}/build/android/${ABI}" ]]; then
	echo -e "\033[0;32mBuild: ${TMIV_DIR} \033[0m";
	mkdir -p ${TMIV_DIR}/build/android/${ABI}
	cd ${TMIV_DIR}/build/android/${ABI}
	rm -f CMakeCache.txt
	cmake \
		-G"Ninja" \
		-DCMAKE_MAKE_PROGRAM=${NINJA_EXE} \
		-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
		-DCMAKE_INSTALL_PREFIX=${TMIV_DIR}/${INSTALL_MODE}/android/${ABI} \
		-DCMAKE_BUILD_TYPE=${MODE} \
		-DANDROID_ABI=${ABI} \
		-DANDROID_NATIVE_API_LEVEL=${API} \
		-DFMT_DIR=${FMT_DIR}/${INSTALL_MODE}/android/${ABI}/lib/cmake/fmt \
		-DBUILD_TESTING=OFF \
		-DBUILD_SHARED_LIBS=ON \
		${TMIV_DIR}
	cmake --build . --config ${MODE} --target install --parallel

	cp 	${TMIV_DIR}/${INSTALL_MODE}/android/${ABI}/lib/*.so \
	${PROJECT_DIR}/Output/Android/${MODE}/${ABI}
fi

### tmc2
if [[ "$COMPIL_TMC2" == "ON" || ! -d "${TMC2_DIR}/build/android/${ABI}/${MODE}" ]]; then
	echo -e "\033[0;32mBuild: ${TMC2_DIR} \033[0m";
	mkdir -p ${TMC2_DIR}/build/android/${ABI}/${MODE}
	cd ${TMC2_DIR}/build/android/${ABI}/${MODE}
	cmake \
		-G"Ninja" \
		-DCMAKE_MAKE_PROGRAM=${NINJA_EXE} \
		-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
		-DCMAKE_BUILD_TYPE=${MODE} \
		-DANDROID_ABI=${ABI} \
		-DANDROID_NATIVE_API_LEVEL=${API} \
		-DLINKER=SHARED \
		-DCMAKE_OBJECT_PATH_MAX=512 \
		-DUSE_JMAPP_VIDEO_CODEC=FALSE \
		-DUSE_HMAPP_VIDEO_CODEC=FALSE \
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

	cp 	${TMC2_DIR}/lib/*.so \
	${PROJECT_DIR}/Output/Android/${MODE}/${ABI}
fi

## mpeg haptics rm
if [[ "$COMPIL_MHRM" == "ON" || ! -d "${MHRM_DIR}/build/android/${ABI}/${MODE}" ]]; then
	echo -e "\033[0;32mBuild: ${MHRM_DIR} \033[0m";
	mkdir -p ${MHRM_DIR}/build/android/${ABI}/${MODE}
	cd ${MHRM_DIR}/build/android/${ABI}/${MODE}
	cmake \
		-G"Ninja" \
		-DCMAKE_MAKE_PROGRAM=${NINJA_EXE} \
		-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
		-DCMAKE_BUILD_TYPE=${MODE} \
		-DANDROID_ABI=${ABI} \
		-DANDROID_NATIVE_API_LEVEL=${API} \
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
	mkdir -p ${PROJECT_DIR}/build/android/${ABI}
	cd ${PROJECT_DIR}/build/android/${ABI}
	cmake \
		-G"Ninja" \
		-DCMAKE_MAKE_PROGRAM=${NINJA_EXE} \
		-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
		-DCMAKE_BUILD_TYPE=${MODE} \
		-DANDROID_ABI=${ABI} \
		-DANDROID_NATIVE_API_LEVEL=${API} \
		-Diloj_DIR=${ILOJ_LIBS}/lib/cmake \
		-Dilojj_DIR=${ILOJ_LIBS}/lib/cmake \
		-DILOJ_INC_DIR=${ILOJ_LIBS}/include \
		-DFMT_DIR=${FMT_DIR}/${INSTALL_MODE}/android/${ABI}/lib/cmake/fmt \
		-DTMIV_DIR=${TMIV_DIR}/${INSTALL_MODE}/android/${ABI}/lib/cmake/TMIV \
		-DTMIV_INC_DIR=${TMIV_DIR}/${INSTALL_MODE}/android/${ABI}/include \
		-DTMIV_LIB_DIR=${TMIV_DIR}/${INSTALL_MODE}/android/${ABI}/lib \
		-DTMC2_DIR=${TMC2_DIR} \
		-DTMC2_LIB_DIR=${TMC2_DIR}/lib \
		-DBUILD_MODE=${MODE} \
        -DUSE_DASH=${COMPIL_DASH} \
		-DUSE_STREAMING_LOG="false" \
		${PROJECT_DIR}
	cmake --build . --config ${MODE} --target install --parallel

	cd ${PROJECT_DIR}/Scripts
fi
