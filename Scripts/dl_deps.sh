#!/bin/bash

trap exit ERR

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

mkdir -p ${SCRIPT_DIR}/../External
cd ${SCRIPT_DIR}/../External

NINJA_VER=1.11.1
FMT_VER=11.0.2
ZLIB_VER=1.3.1
GLFW_VER=3.4
TMIV_VER=23.0
TMC2_VER=25.0
MHRM_VER=5.1

# get ninja
NINJA_DIR=ninja/${NINJA_VER}
if [[ ! -d "${NINJA_DIR}" ]] ; then
	echo --- download ninja ${NINJA_VER}
	mkdir -p ninja/${NINJA_VER}
	curl -L https://github.com/ninja-build/ninja/releases/download/v${NINJA_VER}/ninja-win.zip --output ninja/${NINJA_VER}/ninja-win.zip
	cd ninja/${NINJA_VER}
	unzip -q ninja-win.zip
	rm -f ninja-win.zip
	curl https://raw.githubusercontent.com/ninja-build/ninja/master/COPYING > COPYING
	cd ../..
fi

#get fmt
FMT_DIR=fmt/${FMT_VER}
if [[ ! -d "${FMT_DIR}" ]] ; then
	echo --- download fmt ${FMT_VER}
	git clone --branch ${FMT_VER} https://github.com/fmtlib/fmt.git ${FMT_DIR}
fi

#get zlib
ZLIB_DIR=zlib/${ZLIB_VER}
if [[ ! -d "${ZLIB_DIR}" ]] ; then
	echo --- download zlib ${ZLIB_VER}
	git clone --branch v${ZLIB_VER} https://github.com/madler/zlib.git ${ZLIB_DIR}
fi

#get glfw
GLFW_DIR=glfw/${GLFW_VER}
if [[ ! -d "${GLFW_DIR}" ]] ; then
	echo --- download glfw ${GLFW_VER}
	git clone --branch ${GLFW_VER} https://github.com/glfw/glfw.git ${GLFW_DIR}
fi

#get tmiv
TMIV_DIR=tmiv/${TMIV_VER}
if [[ ! -d "${TMIV_DIR}" ]] ; then
	echo --- download tmiv ${TMIV_VER}
	git clone --branch v${TMIV_VER} https://gitlab.com/mpeg-i-visual/tmiv.git ${TMIV_DIR}
fi

#get tmc2
TMC2_DIR=mpeg-pcc-tmc2/${TMC2_VER}
if [[ ! -d "${TMC2_DIR}" ]] ; then 
	echo --- download tmc2 ${TMC2_VER}
	git clone --branch release-v${TMC2_VER} https://github.com/MPEGGroup/mpeg-pcc-tmc2.git ${TMC2_DIR}
fi

#get mhrm
MHRM_DIR=mpeg-haptics-rm/${MHRM_VER}
if [[ ! -d "${MHRM_DIR}" ]] ; then 
	echo --- download mhrm ${MHRM_VER}
	git clone -b CRM${MHRM_VER} https://github.com/MPEGGroup/HapticReferenceSoftware.git ${MHRM_DIR}
fi

echo --- done

#EOF 
