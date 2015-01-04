
#ifndef __CommonService_h__
#define __CommonService_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
class ServerRequest;

namespace KFS
{

typedef ::std::vector<Int> IntSeq;

class __U__IntSeq { };

typedef ::std::vector<Short> ShortSeq;

class __U__ShortSeq { };

typedef ::std::vector<Long> LongSeq;

class __U__LongSeq { };

typedef ::std::vector<Float> FloatSeq;

class __U__FloatSeq { };

typedef ::std::vector< ::std::string> StrSeq;

class __U__StrSeq { };

struct FileAttr
{
    Long fileid;
    Long mtime;
    Long ctime;
    Long crtime;
    Long chunkCount;
    Long fileSize;
    ::std::string type;
    Short numReplicas;

    bool operator==(const FileAttr&) const;
    bool operator!=(const FileAttr&) const;

    void __write(OutputStream*) const;
    void __read(InputStream*);
};

struct SvrLoc
{
    ::std::string hostname;
    Short port;

    bool operator==(const SvrLoc&) const;
    bool operator!=(const SvrLoc&) const;

    void __write(OutputStream*) const;
    void __read(InputStream*);
};

typedef ::std::vector< ::KFS::SvrLoc> SvrLocSeq;

class __U__SvrLocSeq { };
void __write(OutputStream*, const ::KFS::SvrLoc*, const ::KFS::SvrLoc*, __U__SvrLocSeq);
void __read(InputStream*, SvrLocSeq&, __U__SvrLocSeq);

struct ChunkLayoutInfo
{
    Long fileOffset;
    Long chunkId;
    Long chunkVersion;
    ::KFS::SvrLocSeq chunkServers;

    bool operator==(const ChunkLayoutInfo&) const;
    bool operator!=(const ChunkLayoutInfo&) const;

    void __write(OutputStream*) const;
    void __read(InputStream*);
};

typedef ::std::vector< ::KFS::ChunkLayoutInfo> ChunkLayoutInfoSeq;

class __U__ChunkLayoutInfoSeq { };
void __write(OutputStream*, const ::KFS::ChunkLayoutInfo*, const ::KFS::ChunkLayoutInfo*, __U__ChunkLayoutInfoSeq);
void __read(InputStream*, ChunkLayoutInfoSeq&, __U__ChunkLayoutInfoSeq);

struct ChunkInfo
{
    Long fileId;
    Long chunkId;
    Long chunkVersion;

    bool operator==(const ChunkInfo&) const;
    bool operator!=(const ChunkInfo&) const;

    void __write(OutputStream*) const;
    void __read(InputStream*);
};

typedef ::std::vector< ::KFS::ChunkInfo> ChunkInfoSeq;

class __U__ChunkInfoSeq { };
void __write(OutputStream*, const ::KFS::ChunkInfo*, const ::KFS::ChunkInfo*, __U__ChunkInfoSeq);
void __read(InputStream*, ChunkInfoSeq&, __U__ChunkInfoSeq);

struct WriteInfo
{
    ::KFS::SvrLoc serverLoc;
    Long writeId;

    bool operator==(const WriteInfo&) const;
    bool operator!=(const WriteInfo&) const;

    void __write(OutputStream*) const;
    void __read(InputStream*);
};

typedef ::std::vector< ::KFS::WriteInfo> WriteInfoSeq;

class __U__WriteInfoSeq { };
void __write(OutputStream*, const ::KFS::WriteInfo*, const ::KFS::WriteInfo*, __U__WriteInfoSeq);
void __read(InputStream*, WriteInfoSeq&, __U__WriteInfoSeq);

}

#endif
