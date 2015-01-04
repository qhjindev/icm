
#ifndef __MetaService_h__
#define __MetaService_h__

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

class MetaService : virtual public Object
{
public:

    MetaService() {}

    virtual Int GetAlloc(Long, Long, const ::std::string&, Long&, Long&, ::KFS::SvrLocSeq&) = 0;
    DispatchStatus ___GetAlloc(ServerRequest&);

    virtual Int GetLayout(Long, Int&, ::KFS::ChunkLayoutInfoSeq&) = 0;
    DispatchStatus ___GetLayout(ServerRequest&);

    virtual Int Allocate(Long, Long, const ::std::string&, const ::std::string&, Long&, Long&, ::KFS::SvrLoc&, ::KFS::SvrLocSeq&) = 0;
    DispatchStatus ___Allocate(ServerRequest&);

    virtual Int Truncate(Long, const ::std::string&, Long) = 0;
    DispatchStatus ___Truncate(ServerRequest&);

    virtual Int Lookup(Long, const ::std::string&, ::KFS::FileAttr&) = 0;
    DispatchStatus ___Lookup(ServerRequest&);

    virtual Int Mkdir(Long, const ::std::string&, Long&) = 0;
    DispatchStatus ___Mkdir(ServerRequest&);

    virtual Int Rmdir(Long, const ::std::string&, const ::std::string&) = 0;
    DispatchStatus ___Rmdir(ServerRequest&);

    virtual Int Readdir(Long, ::KFS::StrSeq&) = 0;
    DispatchStatus ___Readdir(ServerRequest&);

    virtual Int ReaddirPlus(Long, Int&) = 0;
    DispatchStatus ___ReaddirPlus(ServerRequest&);

    virtual Int GetDirSummary(Long, Long&, Long&) = 0;
    DispatchStatus ___GetDirSummary(ServerRequest&);

    virtual Int Create(Long, const ::std::string&, Int, bool, Long&) = 0;
    DispatchStatus ___Create(ServerRequest&);

    virtual Int Remove(Long, const ::std::string&, const ::std::string&) = 0;
    DispatchStatus ___Remove(ServerRequest&);

    virtual Int Rename(Long, const ::std::string&, const ::std::string&, bool, ::std::string&) = 0;
    DispatchStatus ___Rename(ServerRequest&);

    virtual Int LeaseAcquire(Long, const ::std::string&, Long&) = 0;
    DispatchStatus ___LeaseAcquire(ServerRequest&);

    virtual Int LeaseRenew(Long, Int, Long, const ::std::string&) = 0;
    DispatchStatus ___LeaseRenew(ServerRequest&);

    virtual Int LeaseRelinquish(Long, Int, Long) = 0;
    DispatchStatus ___LeaseRelinquish(ServerRequest&);

    virtual Int ChangeFileReplication(Long, Short) = 0;
    DispatchStatus ___ChangeFileReplication(ServerRequest&);

    virtual Int DumpChunkServerMap() = 0;
    DispatchStatus ___DumpChunkServerMap(ServerRequest&);

    virtual Int UpServers() = 0;
    DispatchStatus ___UpServers(ServerRequest&);

    virtual DispatchStatus __dispatch(ServerRequest& in);
};

class ChunkMetaService : virtual public Object
{
public:

    ChunkMetaService() {}

    virtual Int Hello(const ::KFS::SvrLoc&, Long, Long, Int, Int, const ::KFS::ChunkInfoSeq&) = 0;
    DispatchStatus ___Hello(ServerRequest&);

    virtual Int CorruptChunk(Long, Long) = 0;
    DispatchStatus ___CorruptChunk(ServerRequest&);

    virtual Int LeaseRenew(Long, Int, Long) = 0;
    DispatchStatus ___LeaseRenew(ServerRequest&);

    virtual Int LeaseRelinquish(Long, Int, Long) = 0;
    DispatchStatus ___LeaseRelinquish(ServerRequest&);

    virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace KFS
{

class MetaService : virtual public IcmProxy::Object
{
public:

    Int GetAlloc(Long fid, Long fileOffset, const ::std::string& filename, Long& chunkId, Long& chunkVersion, ::KFS::SvrLocSeq& locations);

    Int GetLayout(Long fid, Int& numChunks, ::KFS::ChunkLayoutInfoSeq& chunks);

    Int Allocate(Long fid, Long fileOffset, const ::std::string& pathname, const ::std::string& clientHost, Long& chunkId, Long& chunkVersion, ::KFS::SvrLoc& masterServer, ::KFS::SvrLocSeq& chunkServers);

    Int Truncate(Long fid, const ::std::string& pathname, Long fileOffset);

    Int Lookup(Long parentFid, const ::std::string& name, ::KFS::FileAttr& fattr);

    Int Mkdir(Long parentFid, const ::std::string& dirname, Long& fileId);

    Int Rmdir(Long parentFid, const ::std::string& dirname, const ::std::string& pathname);

    Int Readdir(Long fid, ::KFS::StrSeq& result);

    Int ReaddirPlus(Long fid, Int& numEntries);

    Int GetDirSummary(Long fid, Long& numFiles, Long& numBytes);

    Int Create(Long parentFid, const ::std::string& filename, Int numReplicas, bool exclusive, Long& fileId);

    Int Remove(Long parentFid, const ::std::string& filename, const ::std::string& pathname);

    Int Rename(Long parentFid, const ::std::string& oldname, const ::std::string& oldpath, bool overwrite, ::std::string& newpath);

    Int LeaseAcquire(Long chunkId, const ::std::string& pathname, Long& leaseId);

    Int LeaseRenew(Long chunkId, Int leaseType, Long leaseId, const ::std::string& pathname);

    Int LeaseRelinquish(Long chunkId, Int leaseType, Long leaseId);

    Int ChangeFileReplication(Long fid, Short numReplicas);

    Int DumpChunkServerMap();

    Int UpServers();
};

class ChunkMetaService : virtual public IcmProxy::Object
{
public:

    Int Hello(const ::KFS::SvrLoc& location, Long totalSpace, Long usedSpace, Int rackId, Int numChunks, const ::KFS::ChunkInfoSeq& chunks);

    Int CorruptChunk(Long fid, Long chunkId);

    Int LeaseRenew(Long chunkId, Int leaseType, Long leaseId);

    Int LeaseRelinquish(Long chunkId, Int leaseType, Long leaseId);
};

}

}

#endif
