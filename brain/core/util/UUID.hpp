/*  Elysia Utilities -- Elysia Synchronization Utilities
 *  UUID.hpp
 *
 *  Copyright (c) 2009, Daniel Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Elysia nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ELYSIA_UUID_HPP_
#define _ELYSIA_UUID_HPP_

#include <sirikata/core/util/Platform.hpp>

namespace boost {
namespace uuids {
struct uuid;
}
}

namespace Elysia {
class ELYSIA_EXPORT UUID : public TotallyOrdered<UUID>
{
public:
    enum {static_size=16};
    typedef unsigned char byte;
    typedef Array<byte,static_size,true> Data;
    typedef Data::iterator iterator;
    typedef Data::const_iterator const_iterator;
private:
    Data mData;
public:
    UUID(const boost::uuids::uuid&);
    UUID() {}
    UUID (const byte *data,
          unsigned int length){
        mData.memcpy(data,length);
    }
    UUID(const Data data):mData(data) {
    }
    static const UUID& null() {
        static unsigned char data[static_size]={0};
        static UUID retval(data,static_size);
        return retval;
    }
    static const UUID& max() {
        static unsigned char data[static_size]=
            {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
        static UUID retval(data,static_size);
        return retval;
    }
    /**
     * Interprets the human readable UUID string using boost functions
     */
    class HumanReadable{};
    class HexString{};
    class BinaryString{};
    UUID(const std::string&,HumanReadable);
    UUID(const std::string&,HexString);
    UUID(const std::string&s,BinaryString ){
        mData.memcpy(s.data(),s.length());
    }
    class GenerateRandom{};
    UUID(GenerateRandom);

    /// Treats an uint32 as a UUID directly. Remaining bytes will be 0.
    explicit UUID(const uint32 v);

    static UUID random();
    const Data& getArray()const{return mData;}
    UUID & operator=(const UUID & other) { mData = other.mData; return *this; }
    UUID & operator=(const Data & other) { mData = other; return *this; }
    bool operator<(const UUID &other)const {return mData < other.mData;}
    bool operator==(const UUID &other)const {return mData == other.mData;}
    bool isNull()const{return mData==Data::null();}
    size_t hash() const;
    class Hasher{public:
        size_t operator() (const UUID&uuid) const {
            return uuid.hash();
        }
    };
    class Null {
    public:
        const UUID& operator()() const {
            return UUID::null();
        }
    };
    class Random {
    public:
        UUID operator()() const {
            return UUID::random();
        }
    };
    std::string rawData()const;
    std::string rawHexData()const;
    std::string readableHexData()const;
    inline std::string toString()const {
      return readableHexData();
    }

    // These are accessor methods instead of cast because we can't currently
    // make casts explicit
    uint32 asUInt32() const;
};

template <> class OptionValueType<UUID> {public:
    static Any lexical_cast(const std::string &value){
        return UUID(value,UUID::HumanReadable());
    }
};

ELYSIA_FUNCTION_EXPORT std::istream & operator>>(std::istream & is, Elysia::UUID & uuid);
ELYSIA_FUNCTION_EXPORT std::ostream &  operator<<(std::ostream & os, const Elysia::UUID & uuid);
}

#endif //_ELYSIA_UUID_HPP_
