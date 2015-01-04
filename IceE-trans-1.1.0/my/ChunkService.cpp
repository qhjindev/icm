
#include <algorithm>
#include <string>
#include "ChunkService.h"
#include <icm/ServerRequest.h>
#include <icc/CdrStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

static const ::std::string __KFS__MetaChunkService_ids[2] =
{
    "::Ice::Object",
    "::KFS::MetaChunkService"
};

DispatchStatus
KFS::MetaChunkService::___Heartbeat(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long totalSpace;
    Long usedSpace;
    Long numChunks;
    __is.read_long(totalSpace);
    __is.read_long(usedSpace);
    __is.read_long(numChunks);
    Int __ret = Heartbeat(totalSpace, usedSpace, numChunks);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaChunkService::___AllocateChunk(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    Long chunkId;
    Long chunkVersion;
    Long leaseId;
    __is.read_long(fid);
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    __is.read_long(leaseId);
    Int __ret = AllocateChunk(fid, chunkId, chunkVersion, leaseId);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaChunkService::___DeleteChunk(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    __is.read_long(chunkId);
    Int __ret = DeleteChunk(chunkId);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaChunkService::___TruncateChunk(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Int chunkSize;
    __is.read_long(chunkId);
    __is.read_int(chunkSize);
    Int __ret = TruncateChunk(chunkId, chunkSize);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaChunkService::___StaleChunks(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    ::KFS::LongSeq staleChunkIds;
    staleChunkIds = __is->readLongSeq();
    Int __ret = StaleChunks(staleChunkIds);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaChunkService::___ChangeChunkVers(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    Long chunkId;
    Long chunkVersion;
    __is.read_long(fid);
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    Int __ret = ChangeChunkVers(fid, chunkId, chunkVersion);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaChunkService::___ReplicateChunk(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    Long chunkId;
    Long chunkVersion;
    ::KFS::SvrLoc srcLocation;
    __is.read_long(fid);
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    srcLocation.__read(__is);
    Int __ret = ReplicateChunk(fid, chunkId, chunkVersion, srcLocation);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaChunkService::___Size(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long fid;
    Long chunkId;
    __is.read_long(fid);
    __is.read_long(chunkId);
    Long chunkSize;
    Int __ret = Size(fid, chunkId, chunkSize);
    __inS.initReply ();
    __os.write_long(chunkSize);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::MetaChunkService::___Retire(ServerRequest& __inS)
{
    OutputStream& __os = __inS.outgoing();
    Int __ret = Retire();
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

static ::std::string __KFS__MetaChunkService_all[] =
{
    "AllocateChunk",
    "ChangeChunkVers",
    "DeleteChunk",
    "Heartbeat",
    "ReplicateChunk",
    "Retire",
    "Size",
    "StaleChunks",
    "TruncateChunk"
};

DispatchStatus
KFS::MetaChunkService::__dispatch(ServerRequest& in)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__KFS__MetaChunkService_all, __KFS__MetaChunkService_all + 9, in.operation());
    if(r.first == r.second)
    {
	return DispatchOperationNotExist;
    }

    switch(r.first - __KFS__MetaChunkService_all)
    {
	case 0:
	{
	    return ___AllocateChunk(in);
	}
	case 1:
	{
	    return ___ChangeChunkVers(in);
	}
	case 2:
	{
	    return ___DeleteChunk(in);
	}
	case 3:
	{
	    return ___Heartbeat(in);
	}
	case 4:
	{
	    return ___ReplicateChunk(in);
	}
	case 5:
	{
	    return ___Retire(in);
	}
	case 6:
	{
	    return ___Size(in);
	}
	case 7:
	{
	    return ___StaleChunks(in);
	}
	case 8:
	{
	    return ___TruncateChunk(in);
	}
    }

    assert(false);
    return DispatchOperationNotExist;
}

static const ::std::string __KFS__ClientChunkService_ids[2] =
{
    "::Ice::Object",
    "::KFS::ClientChunkService"
};

DispatchStatus
KFS::ClientChunkService::___Open(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Int openFlags;
    __is.read_long(chunkId);
    __is.read_int(openFlags);
    Int __ret = Open(chunkId, openFlags);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ClientChunkService::___Close(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    __is.read_long(chunkId);
    Int __ret = Close(chunkId);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ClientChunkService::___Read(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Long chunkVersion;
    Long offset;
    Int numBytes;
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    __is.read_long(offset);
    __is.read_int(numBytes);
    ::KFS::IntSeq chunksums;
    Float diskIoTime;
    Float elapsedTime;
    Int __ret = Read(chunkId, chunkVersion, offset, numBytes, chunksums, diskIoTime, elapsedTime);
    __inS.initReply ();
    __os->writeIntSeq(chunksums);
    __os.write_float(diskIoTime);
    __os.write_float(elapsedTime);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ClientChunkService::___WriteIdAlloc(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Long chunkVersion;
    Long offset;
    Int numBytes;
    ::KFS::SvrLocSeq chunkServerLoc;
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    __is.read_long(offset);
    __is.read_int(numBytes);
    ::KFS::__read(__is, chunkServerLoc, ::KFS::__U__SvrLocSeq());
    ::std::string writeIdStr;
    Int __ret = WriteIdAlloc(chunkId, chunkVersion, offset, numBytes, chunkServerLoc, writeIdStr);
    __inS.initReply ();
    __os.write_string(writeIdStr);
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ClientChunkService::___WritePrepare(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Long chunkVersion;
    Long offset;
    Int numBytes;
    Int chunkSum;
    ::KFS::WriteInfoSeq writeInfos;
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    __is.read_long(offset);
    __is.read_int(numBytes);
    __is.read_int(chunkSum);
    ::KFS::__read(__is, writeInfos, ::KFS::__U__WriteInfoSeq());
    Int __ret = WritePrepare(chunkId, chunkVersion, offset, numBytes, chunkSum, writeInfos);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ClientChunkService::___WritePrepareFwd(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    ::std::string writeIdStr;
    __is.read_string(writeIdStr);
    Int __ret = WritePrepareFwd(writeIdStr);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ClientChunkService::___WriteSync(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Long chunkVersion;
    ::KFS::WriteInfoSeq writeInfos;
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    ::KFS::__read(__is, writeInfos, ::KFS::__U__WriteInfoSeq());
    Int __ret = WriteSync(chunkId, chunkVersion, writeInfos);
    __inS.initReply ();
    __os.write_int(__ret);
    return DispatchOK;
}

DispatchStatus
KFS::ClientChunkService::___Size(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Long chunkId;
    Long chunkVersion;
    __is.read_long(chunkId);
    __is.read_long(chunkVersion);
    Long sz;
    Int __ret = Size(chunkId, chunkVersion, sz);
    __inS.initReply ();
    __os.write_long(sz);
    __os.write_int(__ret);
    return DispatchOK;
}

static ::std::string __KFS__ClientChunkService_all[] =
{
    "Close",
    "Open",
    "Read",
    "Size",
    "WriteIdAlloc",
    "WritePrepare",
    "WritePrepareFwd",
    "WriteSync"
};

DispatchStatus
KFS::ClientChunkService::__dispatch(ServerRequest& in)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__KFS__ClientChunkService_all, __KFS__ClientChunkService_all + 8, in.operation());
    if(r.first == r.second)
    {
	return DispatchOperationNotExist;
    }

    switch(r.first - __KFS__ClientChunkService_all)
    {
	case 0:
	{
	    return ___Close(in);
	}
	case 1:
	{
	    return ___Open(in);
	}
	case 2:
	{
	    return ___Read(in);
	}
	case 3:
	{
	    return ___Size(in);
	}
	case 4:
	{
	    return ___WriteIdAlloc(in);
	}
	case 5:
	{
	    return ___WritePrepare(in);
	}
	case 6:
	{
	    return ___WritePrepareFwd(in);
	}
	case 7:
	{
	    return ___WriteSync(in);
	}
    }

    assert(false);
    return DispatchOperationNotExist;
}

Int
IcmProxy::KFS::MetaChunkService::Heartbeat(Long totalSpace, Long usedSpace, Long numChunks)
{
    static const char* __operation("Heartbeat");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(totalSpace);
    __os.write_long(usedSpace);
    __os.write_long(numChunks);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaChunkService::AllocateChunk(Long fid, Long chunkId, Long chunkVersion, Long leaseId)
{
    static const char* __operation("AllocateChunk");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
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
IcmProxy::KFS::MetaChunkService::DeleteChunk(Long chunkId)
{
    static const char* __operation("DeleteChunk");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
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
IcmProxy::KFS::MetaChunkService::TruncateChunk(Long chunkId, Int chunkSize)
{
    static const char* __operation("TruncateChunk");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_int(chunkSize);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaChunkService::StaleChunks(const ::KFS::LongSeq& staleChunkIds)
{
    static const char* __operation("StaleChunks");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os->writeLongSeq(staleChunkIds);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaChunkService::ChangeChunkVers(Long fid, Long chunkId, Long chunkVersion)
{
    static const char* __operation("ChangeChunkVers");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaChunkService::ReplicateChunk(Long fid, Long chunkId, Long chunkVersion, const ::KFS::SvrLoc& srcLocation)
{
    static const char* __operation("ReplicateChunk");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(fid);
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    srcLocation.__write(__os);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaChunkService::Size(Long fid, Long chunkId, Long& chunkSize)
{
    static const char* __operation("Size");
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
    __is.read_long(chunkSize);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::MetaChunkService::Retire()
{
    static const char* __operation("Retire");
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
IcmProxy::KFS::ClientChunkService::Open(Long chunkId, Int openFlags)
{
    static const char* __operation("Open");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_int(openFlags);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::ClientChunkService::Close(Long chunkId)
{
    static const char* __operation("Close");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
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
IcmProxy::KFS::ClientChunkService::Read(Long chunkId, Long chunkVersion, Long offset, Int numBytes, ::KFS::IntSeq& chunksums, Float& diskIoTime, Float& elapsedTime)
{
    static const char* __operation("Read");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    __os.write_long(offset);
    __os.write_int(numBytes);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    chunksums = __is->readIntSeq();
    __is.read_float(diskIoTime);
    __is.read_float(elapsedTime);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::ClientChunkService::WriteIdAlloc(Long chunkId, Long chunkVersion, Long offset, Int numBytes, const ::KFS::SvrLocSeq& chunkServerLoc, ::std::string& writeIdStr)
{
    static const char* __operation("WriteIdAlloc");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    __os.write_long(offset);
    __os.write_int(numBytes);
    if(chunkServerLoc.size() == 0)
    {
	__os.write_short(0);
    }
    else
    {
	::KFS::__write(__os, &chunkServerLoc[0], &chunkServerLoc[0] + chunkServerLoc.size(), ::KFS::__U__SvrLocSeq());
    }
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_string(writeIdStr);
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::ClientChunkService::WritePrepare(Long chunkId, Long chunkVersion, Long offset, Int numBytes, Int chunkSum, const ::KFS::WriteInfoSeq& writeInfos)
{
    static const char* __operation("WritePrepare");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    __os.write_long(offset);
    __os.write_int(numBytes);
    __os.write_int(chunkSum);
    if(writeInfos.size() == 0)
    {
	__os.write_short(0);
    }
    else
    {
	::KFS::__write(__os, &writeInfos[0], &writeInfos[0] + writeInfos.size(), ::KFS::__U__WriteInfoSeq());
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
IcmProxy::KFS::ClientChunkService::WritePrepareFwd(const ::std::string& writeIdStr)
{
    static const char* __operation("WritePrepareFwd");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_string(writeIdStr);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_int(__ret);
    return __ret;
}

Int
IcmProxy::KFS::ClientChunkService::WriteSync(Long chunkId, Long chunkVersion, const ::KFS::WriteInfoSeq& writeInfos)
{
    static const char* __operation("WriteSync");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    if(writeInfos.size() == 0)
    {
	__os.write_short(0);
    }
    else
    {
	::KFS::__write(__os, &writeInfos[0], &writeInfos[0] + writeInfos.size(), ::KFS::__U__WriteInfoSeq());
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
IcmProxy::KFS::ClientChunkService::Size(Long chunkId, Long chunkVersion, Long& sz)
{
    static const char* __operation("Size");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_long(chunkId);
    __os.write_long(chunkVersion);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return -1;
    InputStream& __is = _invocation.inpStream();
    Int __ret;
    __is.read_long(sz);
    __is.read_int(__ret);
    return __ret;
}
