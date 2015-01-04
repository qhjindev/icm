
#include <algorithm>
#include <string>
#include "MetaService.h"
#include <icm/ServerRequest.h>
#include <icc/CdrStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

static const ::std::string __KFS__MetaService_ids[2] =
{
    "::Ice::Object",
    "::KFS::MetaService"
};

DispatchStatus
KFS::MetaService::___GetAlloc(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    Long fileOffset;
    ::std::string filename;
    __is.read_long(fid);
    __is.read_long(fileOffset);
    __is.read_string(filename);
    Long chunkId;
    Long chunkVersion;
    ::KFS::SvrLocSeq locations;
    Int __ret = GetAlloc(fid, fileOffset, filename, chunkId, chunkVersion, locations);
    __inS.initReply ();
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    if(locations.size() == 0)
    {
	__os.write_short(0);
    }
    else
    {
	::KFS::__write(__os, &locations[0], &locations[0] + locations.size(), ::KFS::__U__SvrLocSeq());
    }
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___GetLayout(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    __is.read_long(fid);
    Int numChunks;
    ::KFS::ChunkLayoutInfoSeq chunks;
    Int __ret = GetLayout(fid, numChunks, chunks);
    __inS.initReply ();
    __os.write_int(numChunks);
    if(chunks.size() == 0)
    {
	__os.write_short(0);
    }
    else
    {
	::KFS::__write(__os, &chunks[0], &chunks[0] + chunks.size(), ::KFS::__U__ChunkLayoutInfoSeq());
    }
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Allocate(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    Long fileOffset;
    ::std::string pathname;
    ::std::string clientHost;
    __is.read_long(fid);
    __is.read_long(fileOffset);
    __is.read_string(pathname);
    __is.read_string(clientHost);
    Long chunkId;
    Long chunkVersion;
    ::KFS::SvrLoc masterServer;
    ::KFS::SvrLocSeq chunkServers;
    Int __ret = Allocate(fid, fileOffset, pathname, clientHost, chunkId, chunkVersion, masterServer, chunkServers);
    __inS.initReply ();
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    masterServer.__write(__os);
    if(chunkServers.size() == 0)
    {
	__os.write_short(0);
    }
    else
    {
	::KFS::__write(__os, &chunkServers[0], &chunkServers[0] + chunkServers.size(), ::KFS::__U__SvrLocSeq());
    }
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Truncate(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    ::std::string pathname;
    Long fileOffset;
    __is.read_long(fid);
    __is.read_string(pathname);
    __is.read_long(fileOffset);
    Int __ret = Truncate(fid, pathname, fileOffset);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Lookup(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long parentFid;
    ::std::string name;
    __is.read_long(parentFid);
    __is.read_string(name);
    ::KFS::FileAttr fattr;
    Int __ret = Lookup(parentFid, name, fattr);
    __inS.initReply ();
    fattr.__write(__os);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Mkdir(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long parentFid;
    ::std::string dirname;
    __is.read_long(parentFid);
    __is.read_string(dirname);
    Long fileId;
    Int __ret = Mkdir(parentFid, dirname, fileId);
    __inS.initReply ();
    __os.write_long(fileId);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Rmdir(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long parentFid;
    ::std::string dirname;
    ::std::string pathname;
    __is.read_long(parentFid);
    __is.read_string(dirname);
    __is.read_string(pathname);
    Int __ret = Rmdir(parentFid, dirname, pathname);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Readdir(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    __is.read_long(fid);
    ::KFS::StrSeq result;
    Int __ret = Readdir(fid, result);
    __inS.initReply ();
    __os->writeStringSeq(result);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___ReaddirPlus(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    __is.read_long(fid);
    Int numEntries;
    Int __ret = ReaddirPlus(fid, numEntries);
    __inS.initReply ();
    __os.write_int(numEntries);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___GetDirSummary(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    __is.read_long(fid);
    Long numFiles;
    Long numBytes;
    Int __ret = GetDirSummary(fid, numFiles, numBytes);
    __inS.initReply ();
    __os.write_long(numFiles);
    __os.write_long(numBytes);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Create(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long parentFid;
    ::std::string filename;
    Int numReplicas;
    bool exclusive;
    __is.read_long(parentFid);
    __is.read_string(filename);
    __is.read_int(numReplicas);
    __is.read_boolean(exclusive);
    Long fileId;
    Int __ret = Create(parentFid, filename, numReplicas, exclusive, fileId);
    __inS.initReply ();
    __os.write_long(fileId);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Remove(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long parentFid;
    ::std::string filename;
    ::std::string pathname;
    __is.read_long(parentFid);
    __is.read_string(filename);
    __is.read_string(pathname);
    Int __ret = Remove(parentFid, filename, pathname);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___Rename(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long parentFid;
    ::std::string oldname;
    ::std::string oldpath;
    bool overwrite;
    __is.read_long(parentFid);
    __is.read_string(oldname);
    __is.read_string(oldpath);
    __is.read_boolean(overwrite);
    ::std::string newpath;
    Int __ret = Rename(parentFid, oldname, oldpath, overwrite, newpath);
    __inS.initReply ();
    __os.write_string(newpath);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___LeaseAcquire(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    ::std::string pathname;
    __is.read_long(chunkId);
    __is.read_string(pathname);
    Long leaseId;
    Int __ret = LeaseAcquire(chunkId, pathname, leaseId);
    __inS.initReply ();
    __os.write_long(leaseId);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___LeaseRenew(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Int leaseType;
    Long leaseId;
    ::std::string pathname;
    __is.read_long(chunkId);
    __is.read_int(leaseType);
    __is.read_long(leaseId);
    __is.read_string(pathname);
    Int __ret = LeaseRenew(chunkId, leaseType, leaseId, pathname);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___LeaseRelinquish(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Int leaseType;
    Long leaseId;
    __is.read_long(chunkId);
    __is.read_int(leaseType);
    __is.read_long(leaseId);
    Int __ret = LeaseRelinquish(chunkId, leaseType, leaseId);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___ChangeFileReplication(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    Short numReplicas;
    __is.read_long(fid);
    __is.read_short(numReplicas);
    Int __ret = ChangeFileReplication(fid, numReplicas);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___DumpChunkServerMap(ServerRequest& __inS)
{
    OutputStream& __os = __inS.outgoing();
    Int __ret = DumpChunkServerMap();
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaService::___UpServers(ServerRequest& __inS)
{
    OutputStream& __os = __inS.outgoing();
    Int __ret = UpServers();
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

static ::std::string __KFS__MetaService_all[] =
{
    "Allocate",
    "ChangeFileReplication",
    "Create",
    "DumpChunkServerMap",
    "GetAlloc",
    "GetDirSummary",
    "GetLayout",
    "LeaseAcquire",
    "LeaseRelinquish",
    "LeaseRenew",
    "Lookup",
    "Mkdir",
    "Readdir",
    "ReaddirPlus",
    "Remove",
    "Rename",
    "Rmdir",
    "Truncate",
    "UpServers"
};

DispatchStatus
KFS::MetaService::__dispatch(ServerRequest& in)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__KFS__MetaService_all, __KFS__MetaService_all + 19, in.operation());
    if(r.first == r.second)
    {
	return DispatchOperationNotExist;
    }

    switch(r.first - __KFS__MetaService_all)
    {
	case 0:
	{
	    return ___Allocate(in);
	}
	case 1:
	{
	    return ___ChangeFileReplication(in);
	}
	case 2:
	{
	    return ___Create(in);
	}
	case 3:
	{
	    return ___DumpChunkServerMap(in);
	}
	case 4:
	{
	    return ___GetAlloc(in);
	}
	case 5:
	{
	    return ___GetDirSummary(in);
	}
	case 6:
	{
	    return ___GetLayout(in);
	}
	case 7:
	{
	    return ___LeaseAcquire(in);
	}
	case 8:
	{
	    return ___LeaseRelinquish(in);
	}
	case 9:
	{
	    return ___LeaseRenew(in);
	}
	case 10:
	{
	    return ___Lookup(in);
	}
	case 11:
	{
	    return ___Mkdir(in);
	}
	case 12:
	{
	    return ___Readdir(in);
	}
	case 13:
	{
	    return ___ReaddirPlus(in);
	}
	case 14:
	{
	    return ___Remove(in);
	}
	case 15:
	{
	    return ___Rename(in);
	}
	case 16:
	{
	    return ___Rmdir(in);
	}
	case 17:
	{
	    return ___Truncate(in);
	}
	case 18:
	{
	    return ___UpServers(in);
	}
    }

    assert(false);
    return DispatchOperationNotExist;
}

static const ::std::string __KFS__ChunkMetaService_ids[2] =
{
    "::Ice::Object",
    "::KFS::ChunkMetaService"
};

DispatchStatus
KFS::ChunkMetaService::___Hello(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    ::KFS::SvrLoc location;
    Long totalSpace;
    Long usedSpace;
    Int rackId;
    Int numChunks;
    ::KFS::ChunkInfoSeq chunks;
    location.__read(__is);
    __is.read_long(totalSpace);
    __is.read_long(usedSpace);
    __is.read_int(rackId);
    __is.read_int(numChunks);
    ::KFS::__read(__is, chunks, ::KFS::__U__ChunkInfoSeq());
    Int __ret = Hello(location, totalSpace, usedSpace, rackId, numChunks, chunks);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ChunkMetaService::___CorruptChunk(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    Long chunkId;
    __is.read_long(fid);
    __is.read_long(chunkId);
    Int __ret = CorruptChunk(fid, chunkId);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ChunkMetaService::___LeaseRenew(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Int leaseType;
    Long leaseId;
    __is.read_long(chunkId);
    __is.read_int(leaseType);
    __is.read_long(leaseId);
    Int __ret = LeaseRenew(chunkId, leaseType, leaseId);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ChunkMetaService::___LeaseRelinquish(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Int leaseType;
    Long leaseId;
    __is.read_long(chunkId);
    __is.read_int(leaseType);
    __is.read_long(leaseId);
    Int __ret = LeaseRelinquish(chunkId, leaseType, leaseId);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

static ::std::string __KFS__ChunkMetaService_all[] =
{
    "CorruptChunk",
    "Hello",
    "LeaseRelinquish",
    "LeaseRenew"
};

DispatchStatus
KFS::ChunkMetaService::__dispatch(ServerRequest& in)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__KFS__ChunkMetaService_all, __KFS__ChunkMetaService_all + 4, in.operation());
    if(r.first == r.second)
    {
	return DispatchOperationNotExist;
    }

    switch(r.first - __KFS__ChunkMetaService_all)
    {
	case 0:
	{
	    return ___CorruptChunk(in);
	}
	case 1:
	{
	    return ___Hello(in);
	}
	case 2:
	{
	    return ___LeaseRelinquish(in);
	}
	case 3:
	{
	    return ___LeaseRenew(in);
	}
    }

    assert(false);
    return DispatchOperationNotExist;
}

Int
IcmProxy::KFS::MetaService::GetAlloc(Long fid, Long fileOffset, const ::std::string& filename, Long& chunkId, Long& chunkVersion, ::KFS::SvrLocSeq& locations)
{
    static const char* __operation("GetAlloc");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    __os.write_long(fileOffset);
    __os.write_string(filename);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    ::KFS::__read(__is, locations, ::KFS::__U__SvrLocSeq());
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::GetLayout(Long fid, Int& numChunks, ::KFS::ChunkLayoutInfoSeq& chunks)
{
    static const char* __operation("GetLayout");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(numChunks);
    ::KFS::__read(__is, chunks, ::KFS::__U__ChunkLayoutInfoSeq());
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Allocate(Long fid, Long fileOffset, const ::std::string& pathname, const ::std::string& clientHost, Long& chunkId, Long& chunkVersion, ::KFS::SvrLoc& masterServer, ::KFS::SvrLocSeq& chunkServers)
{
    static const char* __operation("Allocate");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    __os.write_long(fileOffset);
    __os.write_string(pathname);
    __os.write_string(clientHost);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    masterServer.__read(__is);
    ::KFS::__read(__is, chunkServers, ::KFS::__U__SvrLocSeq());
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Truncate(Long fid, const ::std::string& pathname, Long fileOffset)
{
    static const char* __operation("Truncate");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    __os.write_string(pathname);
    __os.write_long(fileOffset);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Lookup(Long parentFid, const ::std::string& name, ::KFS::FileAttr& fattr)
{
    static const char* __operation("Lookup");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(parentFid);
    __os.write_string(name);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    fattr.__read(__is);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Mkdir(Long parentFid, const ::std::string& dirname, Long& fileId)
{
    static const char* __operation("Mkdir");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(parentFid);
    __os.write_string(dirname);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_long(fileId);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Rmdir(Long parentFid, const ::std::string& dirname, const ::std::string& pathname)
{
    static const char* __operation("Rmdir");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(parentFid);
    __os.write_string(dirname);
    __os.write_string(pathname);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Readdir(Long fid, ::KFS::StrSeq& result)
{
    static const char* __operation("Readdir");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    result = __is->readStringSeq();
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::ReaddirPlus(Long fid, Int& numEntries)
{
    static const char* __operation("ReaddirPlus");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(numEntries);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::GetDirSummary(Long fid, Long& numFiles, Long& numBytes)
{
    static const char* __operation("GetDirSummary");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_long(numFiles);
    __is.read_long(numBytes);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Create(Long parentFid, const ::std::string& filename, Int numReplicas, bool exclusive, Long& fileId)
{
    static const char* __operation("Create");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(parentFid);
    __os.write_string(filename);
    __os.write_int(numReplicas);
    __os.write_boolean(exclusive);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_long(fileId);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Remove(Long parentFid, const ::std::string& filename, const ::std::string& pathname)
{
    static const char* __operation("Remove");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(parentFid);
    __os.write_string(filename);
    __os.write_string(pathname);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::Rename(Long parentFid, const ::std::string& oldname, const ::std::string& oldpath, bool overwrite, ::std::string& newpath)
{
    static const char* __operation("Rename");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(parentFid);
    __os.write_string(oldname);
    __os.write_string(oldpath);
    __os.write_boolean(overwrite);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_string(newpath);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::LeaseAcquire(Long chunkId, const ::std::string& pathname, Long& leaseId)
{
    static const char* __operation("LeaseAcquire");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_string(pathname);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_long(leaseId);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::LeaseRenew(Long chunkId, Int leaseType, Long leaseId, const ::std::string& pathname)
{
    static const char* __operation("LeaseRenew");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_int(leaseType);
    __os.write_long(leaseId);
    __os.write_string(pathname);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::LeaseRelinquish(Long chunkId, Int leaseType, Long leaseId)
{
    static const char* __operation("LeaseRelinquish");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_int(leaseType);
    __os.write_long(leaseId);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::ChangeFileReplication(Long fid, Short numReplicas)
{
    static const char* __operation("ChangeFileReplication");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    __os.write_short(numReplicas);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::DumpChunkServerMap()
{
    static const char* __operation("DumpChunkServerMap");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaService::UpServers()
{
    static const char* __operation("UpServers");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::ChunkMetaService::Hello(const ::KFS::SvrLoc& location, Long totalSpace, Long usedSpace, Int rackId, Int numChunks, const ::KFS::ChunkInfoSeq& chunks)
{
    static const char* __operation("Hello");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    location.__write(__os);
    __os.write_long(totalSpace);
    __os.write_long(usedSpace);
    __os.write_int(rackId);
    __os.write_int(numChunks);
    if(chunks.size() == 0)
    {
	__os.write_short(0);
    }
    else
    {
	::KFS::__write(__os, &chunks[0], &chunks[0] + chunks.size(), ::KFS::__U__ChunkInfoSeq());
    }
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::ChunkMetaService::CorruptChunk(Long fid, Long chunkId)
{
    static const char* __operation("CorruptChunk");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    __os.write_long(chunkId);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::ChunkMetaService::LeaseRenew(Long chunkId, Int leaseType, Long leaseId)
{
    static const char* __operation("LeaseRenew");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_int(leaseType);
    __os.write_long(leaseId);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::ChunkMetaService::LeaseRelinquish(Long chunkId, Int leaseType, Long leaseId)
{
    static const char* __operation("LeaseRelinquish");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_int(leaseType);
    __os.write_long(leaseId);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}
