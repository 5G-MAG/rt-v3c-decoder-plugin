#!/bin/bash

trap exit ERR

if [ $# -lt 3 ]
  then
	echo "ERROR: not enough parameters";
	echo "Usage: copy_libs.sh <dest_dir> <debug|release> <Windows|Android>";
	exit 1;
fi



if [ $# -eq 3 ]
then 
	var=$2
	if [ ${var,,} = debug ] 
	then
		MODE=Debug
	else 
		MODE=Release
	fi

	SC_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

	## Windows
	if [ $3 == "Windows" ]
	then 
		mkdir -p $1
		echo -e "\033[0;32mCopy Windows dll for ${MODE}\033[0m";
		if [ -d ${SC_DIR}/../Output/Windows/${MODE}/x86_64 -a -d ${SC_DIR}/../Libraries/Windows/${MODE}/x86_64 ]; then 
			cp ${SC_DIR}/../Output/Windows/${MODE}/x86_64/*.dll \
			$1
			cp ${SC_DIR}/../Libraries/iloj/${MODE}/msvc/bin/*.dll \
			$1
			cp ${SC_DIR}/../Libraries/Windows/${MODE}/x86_64/*.dll \
			$1
		else
			echo "Source dir missing, the project is probably not built for Windows ${MODE}"
		fi
	fi

	## Android
	if [ $3 == "Android" ]
	then 
		mkdir -p $1/Android/libs/arm64-v8a
		echo -e "\033[0;32mCopy Android so & jar for ${MODE}\033[0m";
		if [ -f ${SC_DIR}/../Libraries/iloj/${MODE}/android/V3CImmersiveDecoderVideo.jar -a -d ${SC_DIR}/../Output/Android/${MODE}/arm64-v8a -a -d ${SC_DIR}/../Libraries/Android/${MODE}/arm64-v8a/ ]; then 

			cp ${SC_DIR}/../Libraries/iloj/${MODE}/android/V3CImmersiveDecoderVideo.jar \
			$1/Android
			cp ${SC_DIR}/../Output/Android/${MODE}/arm64-v8a/*.so \
			$1/Android/libs/arm64-v8a
			cp ${SC_DIR}/../Libraries/iloj/${MODE}/android/arm64-v8a/lib/*.so \
			$1/Android/libs/arm64-v8a
			cp ${SC_DIR}/../Libraries/Android/${MODE}/arm64-v8a/*.so \
			$1/Android/libs/arm64-v8a
		else
			echo "Source dir missing, the project is probably not built for Android ${MODE}"
		fi
	fi

	## Android Studio
	if [ $3 == "AndroidStudio" ]
	then 
		echo -e "\033[0;32mCopy Android Studio so & jar for ${MODE}\033[0m";
		if [ -f ${SC_DIR}/../Libraries/iloj/${MODE}/android/V3CImmersiveDecoderVideo.jar -a -d ${SC_DIR}/../Output/Android/${MODE}/arm64-v8a -a -d ${SC_DIR}/../Libraries/Android/${MODE}/arm64-v8a/ ]; then 

			cp ${SC_DIR}/../Libraries/iloj/${MODE}/android/V3CImmersiveDecoderVideo.jar \
			$1/libs
			cp ${SC_DIR}/../Output/Android/${MODE}/arm64-v8a/*.so \
			$1/src/main/jniLibs/arm64-v8a
			cp ${SC_DIR}/../Libraries/iloj/${MODE}/android/arm64-v8a/lib/*.so \
			$1/src/main/jniLibs/arm64-v8a
			cp ${SC_DIR}/../Libraries/Android/${MODE}/arm64-v8a/*.so \
			$1/src/main/jniLibs/arm64-v8a
		else
			echo "Source dir missing, the project is probably not built for Android ${MODE}"
		fi
	fi
fi
