
#ifndef __ChunkService_h__
#define __ChunkService_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <CommonService.h>
class ServerRequest;

namespace KFS
{

}

namespace KFS
{

class MetaChunkService : virtual public Object
{
public:

    MetaChunkService() {}

    virtual Int Heartbeat(Long, Long, Long) = 0;
    DispatchStatus ___Heartbeat(ServerRequest&);

    virtual Int AllocateChunk(Long, Long, Long, Long) = 0;
    DispatchStatus ___AllocateChunk(ServerRequest&);

    virtual Int DeleteChunk(Long) = 0;
    DispatchStatus ___DeleteChunk(ServerRequest&);

    virtual Int TruncateChunk(Long, Int) = 0;
    DispatchStatus ___TruncateChunk(ServerRequest&);

    virtual Int StaleChunks(const ::KFS::LongSeq&) = 0;
    DispatchStatus ___StaleChunks(ServerRequest&);

    virtual Int ChangeChunkVers(Long, Long, Long) = 0;
    DispatchStatus ___ChangeChunkVers(ServerRequest&);

    virtual Int ReplicateChunk(Long, Long, Long, const ::KFS::SvrLoc&) = 0;
    DispatchStatus ___ReplicateChunk(ServerRequest&);

    virtual Int Size(Long, Long, Long&) = 0;
    DispatchStatus ___Size(ServerRequest&);

    virtual Int Retire() = 0;
    DispatchStatus ___Retire(ServerRequest&);

    virtual DispatchStatus __dispatch(ServerRequest& in);
};

class ClientChunkService : virtual public Object
{
public:

    ClientChunkService() {}

    virtual Int Open(Long, Int) = 0;
    DispatchStatus ___Open(ServerRequest&);

    virtual Int Close(Long) = 0;
    DispatchStatus ___Close(ServerRequest&);

    virtual Int Read(Long, Long, Long, Int, ::KFS::IntSeq&, Float&, Float&) = 0;
    DispatchStatus ___Read(ServerRequest&);

    virtual Int WriteIdAlloc(Long, Long, Long, Int, const ::KFS::SvrLocSeq&, ::std::string&) = 0;
    DispatchStatus ___WriteIdAlloc(ServerRequest&);

    virtual Int WritePrepare(Long, Long, Long, Int, Int, const ::KFS::WriteInfoSeq&) = 0;
    DispatchStatus ___WritePrepare(ServerRequest&);

    virtual Int WritePrepareFwd(const ::std::string&) = 0;
    DispatchStatus ___WritePrepareFwd(ServerRequest&);

    virtual Int WriteSync(Long, Long, const ::KFS::WriteInfoSeq&) = 0;
    DispatchStatus ___WriteSync(ServerRequest&);

    virtual Int Size(Long, Long, Long&) = 0;
    DispatchStatus ___Size(ServerRequest&);

    virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace KFS
{

class MetaChunkService : virtual public IcmProxy::Object
{
public:

    Int Heartbeat(Long totalSpace, Long usedSpace, Long numChunks);

    Int AllocateChunk(Long fid, Long chunkId, Long chunkVersion, Long leaseId);

    Int DeleteChunk(Long chunkId);

    Int TruncateChunk(Long chunkId, Int chunkSize);

    Int StaleChunks(const ::KFS::LongSeq& staleChunkIds);

    Int ChangeChunkVers(Long fid, Long chunkId, Long chunkVersion);

    Int ReplicateChunk(Long fid, Long chunkId, Long chunkVersion, const ::KFS::SvrLoc& srcLocation);

    Int Size(Long fid, Long chunkId, Long& chunkSize);

    Int Retire();
};

class ClientChunkService : virtual public IcmProxy::Object
{
public:

    Int Open(Long chunkId, Int openFlags);

    Int Close(Long chunkId);

    Int Read(Long chunkId, Long chunkVersion, Long offset, Int numBytes, ::KFS::IntSeq& chunksums, Float& diskIoTime, Float& elapsedTime);

    Int WriteIdAlloc(Long chunkId, Long chunkVersion, Long offset, Int numBytes, const ::KFS::SvrLocSeq& chunkServerLoc, ::std::string& writeIdStr);

    Int WritePrepare(Long chunkId, Long chunkVersion, Long offset, Int numBytes, Int chunkSum, const ::KFS::WriteInfoSeq& writeInfos);

    Int WritePrepareFwd(const ::std::string& writeIdStr);

    Int WriteSync(Long chunkId, Long chunkVersion, const ::KFS::WriteInfoSeq& writeInfos);

    Int Size(Long chunkId, Long chunkVersion, Long& sz);
};

}

}

#endif
