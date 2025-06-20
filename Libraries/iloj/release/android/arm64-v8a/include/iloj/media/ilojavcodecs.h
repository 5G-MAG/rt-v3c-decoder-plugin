#pragma once

#include <map>
#include <string>

using namespace std;

namespace iloj::media::AVCodec
{
class IIlojAVCodec;

typedef IIlojAVCodec *(*funcCreateAVCodec)();

class CIlojAVcodecs
{

public:
    CIlojAVcodecs();
    virtual ~CIlojAVcodecs();

    void init(string avcodec_name);
    void exit();

    const map<string, IIlojAVCodec *> &getAVCodecs() { return m_mapAVCodec; }

private:
    map<string, IIlojAVCodec *> m_mapAVCodec;
};
} // namespace iloj::media::AVCodec
